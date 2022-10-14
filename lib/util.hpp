//
// Created by tammd on 10/13/22.
//

#ifndef DTL_UTIL_HPP
#define DTL_UTIL_HPP

#include <string>
#include <algorithm>
namespace http10
{
    namespace server
    {
        std::string to_lower(std::string in);

        uint16_t str_to_u16(std::string in);
        bool str_to_bool(std::string in);
        int str_to_int(std::string in);
    }
}
#endif //DTL_UTIL_HPP
