#pragma once

#include <string>
#include <map>

namespace cppweb {

    struct Response {
        int status_code = 200;
        std::string body;
        std::string file_path; // New field for file streaming
        std::string content_type = "text/plain";
        std::map<std::string, std::string> headers;

        Response() = default;
    };


} // namespace cppweb
