#pragma once

#include "../core/request.hpp"
#include "../core/response.hpp"
#include <map>
#include <functional>
#include <memory>
#include <mutex>

namespace cppweb {

    using RouteHandler = std::function<void(const Request&, Response&)>;

    class Router {
    public:
        Router() = default;
        ~Router() = default;

        // Register GET route
        void get(const std::string& path, RouteHandler handler);

        // Register POST route
        void post(const std::string& path, RouteHandler handler);

        // Register PUT route
        void put(const std::string& path, RouteHandler handler);

        // Register DELETE route
        void del(const std::string& path, RouteHandler handler);

        // Route a request and generate a response
        void route(const Request& req, Response& res) const;

        // Check if a route exists
        bool has_route(const std::string& method, const std::string& path) const;

        // Get total number of registered routes
        size_t route_count() const;

    private:
        std::map<std::string, RouteHandler> get_routes;
        std::map<std::string, RouteHandler> post_routes;
        std::map<std::string, RouteHandler> put_routes;
        std::map<std::string, RouteHandler> delete_routes;
        mutable std::mutex routes_mutex;

        std::map<std::string, RouteHandler>& get_route_map(const std::string& method);
        const std::map<std::string, RouteHandler>& get_route_map_const(const std::string& method) const;
    };

} // namespace cppweb