#include "../include/cppweb.hpp"
#include <iostream>

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
        std::cout << "cppweb server listening on " << port << "...\n";
        
        while (true) {
            // TODO: Implement actual socket binding, listening, and accept loop.
            break;
        }
    }

} // namespace cppweb