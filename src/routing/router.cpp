#include "../../include/cppweb/routing/router.hpp"
#include "../../include/cppweb/utils/http_utils.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace cppweb {

void Router::get(const std::string& path, RouteHandler handler) {
    std::unique_lock<std::mutex> lock(routes_mutex);
    get_routes[path] = handler;
}

void Router::post(const std::string& path, RouteHandler handler) {
    std::unique_lock<std::mutex> lock(routes_mutex);
    post_routes[path] = handler;
}

void Router::put(const std::string& path, RouteHandler handler) {
    std::unique_lock<std::mutex> lock(routes_mutex);
    put_routes[path] = handler;
}

void Router::del(const std::string& path, RouteHandler handler) {
    std::unique_lock<std::mutex> lock(routes_mutex);
    delete_routes[path] = handler;
}

void Router::route(const Request& req, Response& res) const {
    std::unique_lock<std::mutex> lock(routes_mutex);
    
    bool route_found = false;
    
    if (req.method == "GET" && get_routes.count(req.path)) {
        get_routes.at(req.path)(req, res);
        route_found = true;
    } else if (req.method == "POST" && post_routes.count(req.path)) {
        post_routes.at(req.path)(req, res);
        route_found = true;
    } else if (req.method == "PUT" && put_routes.count(req.path)) {
        put_routes.at(req.path)(req, res);
        route_found = true;
    } else if (req.method == "DELETE" && delete_routes.count(req.path)) {
        delete_routes.at(req.path)(req, res);
        route_found = true;
    }

    if (!route_found) {
        res.status_code = 404;
        res.body = "404 Not Found";
        res.content_type = "text/plain";
    }
}

bool Router::has_route(const std::string& method, const std::string& path) const {
    std::unique_lock<std::mutex> lock(routes_mutex);
    
    if (method == "GET") {
        return get_routes.count(path) > 0;
    } else if (method == "POST") {
        return post_routes.count(path) > 0;
    } else if (method == "PUT") {
        return put_routes.count(path) > 0;
    } else if (method == "DELETE") {
        return delete_routes.count(path) > 0;
    }
    return false;
}

size_t Router::route_count() const {
    std::unique_lock<std::mutex> lock(routes_mutex);
    return get_routes.size() + post_routes.size() + put_routes.size() + delete_routes.size();
}

std::map<std::string, RouteHandler>& Router::get_route_map(const std::string& method) {
    if (method == "GET") {
        return get_routes;
    } else if (method == "POST") {
        return post_routes;
    } else if (method == "PUT") {
        return put_routes;
    } else if (method == "DELETE") {
        return delete_routes;
    }
    static std::map<std::string, RouteHandler> empty;
    return empty;
}

const std::map<std::string, RouteHandler>& Router::get_route_map_const(const std::string& method) const {
    if (method == "GET") {
        return get_routes;
    } else if (method == "POST") {
        return post_routes;
    } else if (method == "PUT") {
        return put_routes;
    } else if (method == "DELETE") {
        return delete_routes;
    }
    static const std::map<std::string, RouteHandler> empty;
    return empty;
}

} // namespace cppweb