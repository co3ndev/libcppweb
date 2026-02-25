#pragma once

#include <string>
#include <functional>
#include <map>

namespace cppweb {

    struct Request {
        std::string method;
        std::string path;
        std::string body;
        std::map<std::string, std::string> headers;
    };

    struct Response {
        int status_code = 200;
        std::string body;
        std::string content_type = "text/plain";
    };

    using RouteHandler = std::function<void(const Request&, Response&)>;

    class Server {
    public:
        Server();
        ~Server();

        void get(const std::string& path, RouteHandler handler);
        void post(const std::string& path, RouteHandler handler);

        void listen(std::string host, int port);

    private:
        std::map<std::string, RouteHandler> get_routes;
        std::map<std::string, RouteHandler> post_routes;
    };

} // namespace cppweb