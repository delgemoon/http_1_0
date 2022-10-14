//
// Created by tammd on 10/11/22.
//

#ifndef DTL_STATE_HPP
#define DTL_STATE_HPP

#include <connection.hpp>
#include <memory>
namespace http10
{
    namespace server
    {
        struct connection_state
        {
            connection_state(const std::shared_ptr<connection> conn) : conn_(conn) {}
            std::shared_ptr<connection> conn_;
        };
        struct epoll_state
        {
            uint32_t  event_;
            void* data_;
        };
    }
}
#endif //DTL_STATE_HPP
