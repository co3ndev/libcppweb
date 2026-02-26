#pragma once

#include <string>
#include <map>

namespace cppweb {

    struct Request {
        std::string method;
        std::string path;
        std::string body;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> query_params;
    };

} // namespace cppweb