//
// Created by tammd on 10/13/22.
//

#include <util.hpp>
#include <string>
#include <limits>
#include <stdexcept>
#include <sstream>

namespace http10
{
    namespace server
    {
        std::string to_lower(std::string in)
        {
            std::transform(in.begin(), in.end(), in.begin(), [](const char value) { return std::tolower(value); });
            return in;
        }

        uint16_t str_to_u16(std::string in)
        {
            int int_value = std::stoi(in);
            if(int_value <= static_cast<int>(std::numeric_limits<uint16_t>::max()) && int_value >= 0)
                return static_cast<uint16_t>(int_value);
            throw std::runtime_error("invalid input, the value is not fit in uint16");
        }

        int str_to_int(std::string in)
        {
            int int_value = std::stoi(in);
            return int_value;
        }

        bool str_to_bool(std::string in)
        {
            auto lower_case = to_lower(in);
            if(lower_case == "true")
                return true;
            else if(lower_case == "false")
                return false;
            throw std::runtime_error("invalid input, the value is not fit in bool");
        }
    }
}