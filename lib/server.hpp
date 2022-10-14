//
// Created by tammd on 10/10/22.
//

#ifndef DTL_SERVER_HPP
#define DTL_SERVER_HPP

#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <monitor.hpp>
#include <connection.hpp>
#include <state.hpp>
#include <thread_pool.hpp>
#include <request_handler.hpp>

namespace http10
{
namespace server {


    class server {
    public:


        void run() {
            struct sockaddr_storage claddr;
            monitor_.add_fd(accept_fd, &accept_fd);
            add_interrupt();
            for(;!stop_;) {
                auto events = monitor_.run();
                if (!events.empty()) {
                    for(const epoll_state& evt : events)
                    {

                        if(evt.data_ == &accept_fd) {
                            socklen_t addrlen = sizeof(struct sockaddr_storage);
                            int cfd = accept(accept_fd, (struct sockaddr *) &claddr, &addrlen);
                            //if (cfd != -1)
                            //    ::fcntl(cfd, F_SETFD, O_ASYNC);
                            std::cout << "new fd " << cfd << "\n";
                            connection_state*& info = states_.emplace_back(new connection_state(std::make_shared<connection>(cfd, handler_)));
                            monitor_.add_fd(cfd, info);
                        }
                        else if(evt.data_ == &interrupt_fd)
                        {
                            std::cout << "CTRL-C" << std::endl;
                            stop_ = true;
                            break;
                        }
                        else
                        {
                            auto event = evt.event_;
                            printf(" events: %s%s%s%s%s\n",
                                   (event & EPOLLIN) ? "EPOLLIN " : "",
                                   (event & EPOLLHUP) ? "EPOLLHUP " : "",
                                   (event& EPOLLERR) ? "EPOLLERR " : "",
                                   (event&EPOLLRDHUP) ? "EPOLLRDHUP": "",
                                   (event&EPOLLOUT) ? "EPOLLOUT": "");
                            connection_state* state = (connection_state*) evt.data_;
                            if(event & (EPOLLIN)){
                                if(state)
                                pool_.assign(state->conn_);
                            }
                            else if(event & (EPOLLERR | EPOLLHUP))
                            {
                                state->conn_->set_done(true);
                                monitor_.del_fd(state->conn_->get_fd());
                            }
                            else if(event & EPOLLRDHUP)
                            {
                                state->conn_->set_done(true);
                                monitor_.del_fd(state->conn_->get_fd());
                            }

                        }
                    }
                }
                else
                {
                    if(errno == EINTR)
                    {
                        std::cout << "interrupt\n" << std::endl;
                        break;
                    }
                    std::cout << "timeout size of connection " << states_.size() << "\n";
                    cleanup();
                }
            }
            std::cout << "complete\n";
            handle_stop();
        }

        server(const uint16_t port, const int num_thread, bool is_detach, const std::string doc = "") : port_(port),
                    monitor_(20000, 500), pool_(num_thread, is_detach), handler_(doc) {
            accept_fd = inetListen(0);
        }

        void handle_stop()
        {
            std::vector<connection_state*>::iterator b = states_.begin();
            while(b != states_.end())
            {
                monitor_.del_fd((*b)->conn_->get_fd());
                delete (*b);
                *b = nullptr;
                b = states_.erase(b);
            }
            monitor_.del_fd(accept_fd);

            monitor_.del_fd(interrupt_fd);

            pool_.stop();

        }

    private:
        void add_interrupt()
        {
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGTERM);
            sigaddset(&mask, SIGINT);
            interrupt_fd = signalfd(-1, &mask, 0);
            sigprocmask(SIG_BLOCK, &mask, NULL);
            struct epoll_event ev;
            ev.events = EPOLLIN ;
            ev.data.ptr = &interrupt_fd;
            monitor_.add_fd(interrupt_fd, &interrupt_fd);
        }
        void cleanup()
        {
            std::vector<connection_state*>::iterator b = states_.begin();
            while(b != states_.end())
            {
                if( (*b)->conn_->get_done() || (*b)->conn_->get_error() != 0)
                {
                    std::cout << "Found " << (*b)->conn_->get_fd() << "\n";
                    monitor_.del_fd((*b)->conn_->get_fd());
                    delete (*b);
                    b = states_.erase(b);
                }
                else {
                    if((*b)->conn_->get_fd() != -1)
                        pool_.assign((*b)->conn_);
                    b++;
                }
            }
        }
        int inetListen(int backlog) {
            int sfd, optval;

            sockaddr_in sockaddr_;
            sockaddr_.sin_family = AF_INET;
            sockaddr_.sin_port = htons(port_);
            sockaddr_.sin_addr.s_addr = INADDR_ANY;
            /* Walk through returned list until we find an address structure
               that can be used to successfully create and bind a socket */

            sfd = socket(AF_INET, SOCK_STREAM, 0);

            if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                           sizeof(optval)) == -1) {
                close(sfd);
                return -1;
            }


            if (bind(sfd, (sockaddr*)&sockaddr_, sizeof(sockaddr_)) == -1)
                return -1;

            if (listen(sfd, backlog) == -1) {
                return -1;
            }
            //if (sfd != -1)
             //   ::fcntl(sfd, F_SETFD, O_ASYNC);
            return sfd;
        }
        uint16_t port_;
        int accept_fd = -1;
        monitor monitor_;
        std::vector<connection_state*> states_;
        thread_pool pool_;
        request_handler handler_;
        std::atomic<bool> stop_ = false;
        int interrupt_fd = -1;

    };
}
}
#endif //DTL_SERVER_HPP
