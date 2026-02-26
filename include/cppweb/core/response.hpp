#pragma once

#include <string>
#include <map>

namespace cppweb {

    struct Response {
        int status_code = 200;
        std::string body;
        std::string content_type = "text/plain";
        std::map<std::string, std::string> headers;

        Response() = default;
    };

} // namespace cppweb