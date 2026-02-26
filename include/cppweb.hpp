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
        std::map<std::string, std::string> query_params;
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

        void listen(int port);

    private:
        std::map<std::string, RouteHandler> get_routes;
        std::map<std::string, RouteHandler> post_routes;

        void handle_client(int client_fd);
        Request parse_request(const std::string& raw_data);
        void route_request(const Request& req, Response& res);
        void send_response(int client_fd, const Response& res);
    };

} // namespace cppweb