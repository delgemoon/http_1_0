//
// Created by tammd on 10/10/22.
//

#ifndef DTL_CONNECTION_HPP
#define DTL_CONNECTION_HPP

#include <header.hpp>
#include <request.hpp>
#include <request_parser.hpp>
#include <request_handler.hpp>
#include <array>
#include <reply.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <util.hpp>
#include <cassert>

namespace http10
{
    namespace server
    {
        struct connection
        {
            explicit connection(const int sock_fd, request_handler& handler) : sock_fd_(sock_fd),request_handler_(handler){}

            ssize_t
            readn(int fd, char *buffer, size_t n)
            {
                ssize_t numRead = 0;
                size_t totRead = 0;
                char *buf;
                buf = buffer;
                numRead = read(fd, buf, n - totRead);
                if (numRead == 0)
                    return totRead;
                if (numRead == -1) {
                    if (errno != EINTR)
                        return -1;
                }
                totRead += numRead;
                return totRead;
            }
            ssize_t
            writen(int fd, const char *buffer, size_t n)
            {
                ssize_t numWritten;
                size_t totWritten;
                const char *buf;
                buf = buffer;
                for (totWritten = 0; totWritten < n; ) {
                    numWritten = write(fd, buf, n - totWritten);
                    if (numWritten <= 0) {
                        if (numWritten == -1 && errno == EINTR)
                            continue;
                        else
                            return -1;
                    }
                    totWritten += numWritten;
                    buf += numWritten;
                }
                return totWritten;
            }
            /// Handle completion of a read operation and write
            void start()
            {
                auto total_read = readn(sock_fd_, buffer_.data(), buffer_.size());
                if(!total_read)
                {
                    done_ = true;
                }
                else if(total_read != -1)
                {
                    auto [result, iter] = request_parser_.parse(request_, buffer_.begin(), buffer_.begin() + total_read);
                    if(result == 0 || result == 1) {
                        request_handler_.handle_request(request_, reply_);
                        auto total_write = writen(sock_fd_, reply_.to_buffers().c_str(), reply_.to_buffers().size());
                        if(total_write == -1) {
                            std::cout << "failed to write\n" ;
                            error = -1;
                        }
                        else {
                           if(std::find_if(std::begin(request_.headers), std::end(request_.headers), [](const header& h) { return to_lower(h.name) == "connection" && to_lower(h.value) == "keep-alive"; }) == request_.headers.end())
                            done_ = true;
                        }
                    }
                }
                else
                {
                    std::cout << "weird data or done \n";
                    error = -1;
                }
            }

            void set_done(bool done)
            {
                done_ = done;
            }

            bool get_done() const
            {
                return done_;
            }

            int get_fd() const
            {
                return sock_fd_;
            }

            int get_error() const
            {
                return error;
            }

            virtual ~connection()
            {
                std::cout << "DTOR connection\n";
                close(sock_fd_);
            }

            auto get_tp() const
            {
                return start_;
            }

            void set_thread_idx(int idx)
            {
                thread_assigned = idx;
            }

            int get_thread_idx() const
            {
                return thread_assigned;
            }
        private:
            int sock_fd_;
            /// The handler used to process the incoming request.
            request_handler& request_handler_;

            /// Buffer for incoming data.
            std::array<char, 4096> buffer_;

            /// The incoming request.
            request request_;

            /// The parser for the incoming request.
            request_parser request_parser_;

            /// The reply to be sent back to the client.
            reply reply_;
            int error = 0;
            bool done_ = false;
            std::chrono::time_point<std::chrono::system_clock> start_ = std::chrono::system_clock::now();
            int thread_assigned = -1;
        };
    }
}

#endif //DTL_CONNECTION_HPP
