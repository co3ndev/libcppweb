#pragma once

#include "request.hpp"
#include "response.hpp"
#include "../routing/router.hpp"
#include "../threading/thread_pool.hpp"
#include <memory>
#include <string>

namespace cppweb {

/**
 * @class Server
 * @brief HTTP server for handling requests and routing
 *
 * Manages socket creation, request acceptance, and delegation to
 * worker threads via a thread pool. Routes requests using the Router.
 */
class Server {
public:
    /**
     * @brief Constructor
     * @param num_threads Number of worker threads for the thread pool (default: 4)
     */
    explicit Server(size_t num_threads = 4);

    /**
     * @brief Destructor
     */
    ~Server();

    /**
     * @brief Register a GET route with a handler
     * @param path The URL path
     * @param handler The request handler function
     */
    void get(const std::string& path, RouteHandler handler);

    /**
     * @brief Register a GET route to serve a static file
     * @param path The URL path
     * @param file_path The path to the file to serve
     */
    void get(const std::string& path, const std::string& file_path);

    /**
     * @brief Register a POST route with a handler
     * @param path The URL path
     * @param handler The request handler function
     */
    void post(const std::string& path, RouteHandler handler);

    /**
     * @brief Register a PUT route with a handler
     * @param path The URL path
     * @param handler The request handler function
     */
    void put(const std::string& path, RouteHandler handler);

    /**
     * @brief Register a DELETE route with a handler
     * @param path The URL path
     * @param handler The request handler function
     */
    void del(const std::string& path, RouteHandler handler);

    /**
     * @brief Start listening for incoming connections
     * @param port The port to listen on
     */
    void listen(int port);

private:
    std::unique_ptr<threading::ThreadPool> thread_pool;
    std::unique_ptr<Router> router;

    /**
     * @brief Handle a client connection
     * @param client_fd The client socket file descriptor
     */
    void handle_client(int client_fd);

    /**
     * @brief Send an HTTP response to a client
     * @param client_fd The client socket file descriptor
     * @param res The response to send
     */
    void send_response(int client_fd, const Response& res);
};

} // namespace cppweb
