//
// request_handler.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef DTL_REQUEST_HANDLER_HPP
#define DTL_REQUEST_HANDLER_HPP

#include <string>

namespace http10
{
    namespace server
    {
        struct reply;
        struct request;

/// The common handler for all incoming requests.
        class request_handler
        {
        public:
            /// Construct with a directory containing files to be served.
            explicit request_handler(const std::string& doc_root);

            /// Handle a request and produce a reply.
            void handle_request(const request& req, reply& rep);

        private:
            /// The directory containing the files to be served.
            std::string doc_root_;

            /// Perform URL-decoding on a string. Returns false if the encoding was
            /// invalid.
            static bool url_decode(const std::string& in, std::string& out);
        };
    }
}
#endif //DTL_REQUEST_HANDLER_HPP
