#include "../include/cppweb.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>

namespace {
    std::string get_status_message(int code) {
        switch (code) {
            case 200: return "OK";
            case 400: return "Bad Request";
            case 404: return "Not Found";
            case 500: return "Internal Server Error";
            // Add other status codes as needed
            default:  return "Unknown";
        }
    }
}

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
			
			// Consume the trailing newline from the first line
            std::string line;
            std::getline(request_stream, line);
			
			// Extract query parameters from raw_path
            std::string path = raw_path;
            std::map<std::string, std::string> query_params;
            size_t query_pos = raw_path.find('?');
            
            if (query_pos != std::string::npos) {
                path = raw_path.substr(0, query_pos); // Base path without the query string
                std::string query_string = raw_path.substr(query_pos + 1);
                
                // Split query string by '&'
                std::istringstream query_stream(query_string);
                std::string key_value;
                while (std::getline(query_stream, key_value, '&')) {
                    size_t eq_pos = key_value.find('=');
                    if (eq_pos != std::string::npos) {
                        query_params[key_value.substr(0, eq_pos)] = key_value.substr(eq_pos + 1);
                    } else if (!key_value.empty()) {
                        query_params[key_value] = ""; // Present but has no value
                    }
                }
            }
			
            // Prepare our request and response objects
            Request req{method, path, "", {}};
            Response res;
            
            // Header Parsing
            while (std::getline(request_stream, line) && line != "\r" && !line.empty()) {
                size_t colon_pos = line.find(':');
                if (colon_pos != std::string::npos) {
                    std::string key = line.substr(0, colon_pos);
                    
                    // Skip leading whitespace in the value
                    size_t value_start = line.find_first_not_of(" \t", colon_pos + 1);
                    std::string value = "";
                    if (value_start != std::string::npos) {
                        value = line.substr(value_start);
                        // Clean up trailing '\r' typical in HTTP headers
                        if (!value.empty() && value.back() == '\r') {
                            value.pop_back();
                        }
                    }
                    req.headers[key] = value;
                }
            }

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
            response_stream << "HTTP/1.1 " << res.status_code << " " << get_status_message(res.status_code) << "\r\n"
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