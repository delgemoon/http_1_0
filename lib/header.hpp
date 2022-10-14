//
// header.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DTL_HEADER_HPP
#define DTL_HEADER_HPP

#include <string>

namespace http10
{
    namespace server
    {
        struct header
        {
            std::string name;
            std::string value;
        };
    }
}
#endif //DTL_HEADER_HPP
