//
// Created by tammd on 10/10/22.
//



#ifndef DTL_MONITOR_HPP
#define DTL_MONITOR_HPP

#include <sys/epoll.h>
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <thread>
#include <state.hpp>
#include <condition_variable>
#include <signal.h>
#include <sys/signalfd.h>

namespace http10
{
    namespace server
    {

    class monitor {
    public:
        void add_fd(const int fd, connection_state*& data) const{
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
            ev.data.ptr = data;
            int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
            if (ret == -1)
                throw std::runtime_error("epoll_ctl add file descriptor failed");
        }

        void add_fd(const int fd, int* other) const{
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
            ev.data.ptr = (void*)other;
            int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
            if (ret == -1)
                throw std::runtime_error("epoll_ctl add file descriptor failed");
        }

        void del_fd(const int fd) {
            struct epoll_event ev;
            ev.data.fd = fd;
            int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ev);
            if (ret == -1)
                throw std::runtime_error("epoll_ctl del file descriptor failed");
        }

        std::vector<http10::server::epoll_state>  run() {
            int timeout = 0;
            if (usec_ != 0)
            {
                timeout = (usec_ < 0) ? -1 : ((usec_ - 1) / 1000 + 1);
            }
            epoll_event events[10000];
            int num_events = epoll_wait(epfd_, events, 10000, timeout);
            if(num_events == -1) {
                if(errno == EAGAIN)
                    std::cout << "Retry \n";
                return std::vector<http10::server::epoll_state>();
            }
            std::vector<epoll_state> result(num_events);
            for(int i = 0; i < num_events; i++)
            {
                if(events[i].events)
                    result[i] = epoll_state{.event_ = events->events, .data_ = events->data.ptr};
            }
            return result;
        }


    public:
        explicit monitor(const int sz, const int usec) : size_(sz), usec_(usec) {
            this->epfd_ = epoll_create(sz);
            if (epfd_ == -1)
                throw std::runtime_error("epoll_create failed");
            //if (epfd_ != -1)
            //    ::fcntl(epfd_, F_SETFD, FD_CLOEXEC);

        }
        virtual ~monitor()
        {
            close(signal_fd);
            close(epfd_);
        }

    private:
        int epfd_;
        int size_;
        int usec_;
        int signal_fd;

    };
}
}
#endif //DTL_MONITOR_HPP
