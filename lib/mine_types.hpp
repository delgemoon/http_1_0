//
// mime_types.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef DTL_MINE_TYPE_HPP
#define DTL_MINE_TYPE_HPP
#include <string>

namespace http10
{
    namespace server
    {
        namespace mime_types {

            /// Convert a file extension into a MIME type.
            std::string extension_to_type(const std::string& extension);

        } // namespace mime_types
    }
}
#endif //DTL_MINE_TYPE_HPP
