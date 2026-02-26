#include "../include/cppweb.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>

namespace cppweb {

    Server::Server() {
        // Initialization (e.g., socket setup)
    }

    Server::~Server() {
        // Cleanup
    }

    void Server::get(const std::string& path, RouteHandler handler) {
        get_routes[path] = handler;
    }

    void Server::post(const std::string& path, RouteHandler handler) {
        post_routes[path] = handler;
    }

    void Server::listen(int port) {
        // Socket creation
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
            std::cerr << "Failed to create socket.\n";
            return;
        }
        
        // Bind socket to the port
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Failed to bind to port " << port << ".\n";
            return;
        }
        
        // Start listening
        if (::listen(server_fd, 10) < 0){
        		std::cerr << "Failed to listen.\n";
        		return;
        } else {
        		std::cout << "Started listening on " << port << ".\n";
        }
        
        // Server loop
        while (true) {
            int addrlen = sizeof(address);
            int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (client_fd < 0) continue; // Skip on error

            // Read the raw HTTP request
            char buffer[2048] = {0};
            read(client_fd, buffer, 2048);

            // Parse the first line of the HTTP request (e.g., "GET /api/status HTTP/1.1")
            std::istringstream request_stream(buffer);
            std::string method, path, http_version;
            request_stream >> method >> path >> http_version;

            // Prepare our request and response objects
            Request req{method, path, "", {}};
            Response res;

            // Route matching!
            bool route_found = false;
            if (method == "GET" && get_routes.count(path)) {
                get_routes[path](req, res); // Invokes your lambda function from main.cpp!
                route_found = true;
            } else if (method == "POST" && post_routes.count(path)) {
                post_routes[path](req, res);
                route_found = true;
            }

            // Fallback for 404 Not Found
            if (!route_found) {
                res.status_code = 404;
                res.body = "404 Not Found";
                res.content_type = "text/plain";
            }

            // 5. Construct the raw HTTP response
            std::ostringstream response_stream;
            response_stream << "HTTP/1.1 " << res.status_code << " OK\r\n"
                            << "Content-Type: " << res.content_type << "\r\n"
                            << "Content-Length: " << res.body.length() << "\r\n"
                            << "Connection: close\r\n\r\n"
                            << res.body;

            // Send response back to the client and close the connection
            std::string response_str = response_stream.str();
            send(client_fd, response_str.c_str(), response_str.length(), 0);
            close(client_fd);
        }
    }

} // namespace cppweb