#include "../../include/cppweb/core/server.hpp"
#include "../../include/cppweb/utils/http_utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <stdexcept>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

namespace cppweb {

namespace {
    class ScopedFD {
    public:
        explicit ScopedFD(int fd = -1) : fd_(fd) {}
        ~ScopedFD() { if (fd_ >= 0) ::close(fd_); }

        // Non-copyable
        ScopedFD(const ScopedFD&) = delete;
        ScopedFD& operator=(const ScopedFD&) = delete;

        // Movable
        ScopedFD(ScopedFD&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }
        ScopedFD& operator=(ScopedFD&& other) noexcept {
            if (this != &other) {
                if (fd_ >= 0) ::close(fd_);
                fd_ = other.fd_;
                other.fd_ = -1;
            }
            return *this;
        }

        int get() const { return fd_; }
        bool is_valid() const { return fd_ >= 0; }
        void release() { fd_ = -1; }

    private:
        int fd_;
    };
}

Server::Server(size_t num_threads) {
    thread_pool = std::make_unique<threading::ThreadPool>(num_threads);
    router = std::make_unique<Router>();
}

Server::~Server() = default;

void Server::get(const std::string& path, RouteHandler handler) {
    router->get(path, handler);
}

void Server::get(const std::string& path, const std::string& file_path) {
    router->get(path, [file_path](const Request& req, Response& res) {
        std::filesystem::path fp = file_path;
        if (std::filesystem::exists(fp) && std::filesystem::is_regular_file(fp)) {
            std::ifstream file(fp, std::ios::binary);
            if (file) {
                std::ostringstream oss;
                oss << file.rdbuf();
                res.body = oss.str();
                res.content_type = utils::get_mime_type(fp.extension().string());
                res.status_code = 200;
                return;
            }
        }

        res.status_code = 404;
        res.body = "404 Not Found";
        res.content_type = "text/plain";
    });
}

void Server::post(const std::string& path, RouteHandler handler) {
    router->post(path, handler);
}

void Server::put(const std::string& path, RouteHandler handler) {
    router->put(path, handler);
}

void Server::del(const std::string& path, RouteHandler handler) {
    router->del(path, handler);
}

void Server::listen(int port) {
    ScopedFD server_fd(socket(AF_INET, SOCK_STREAM, 0));
    if (!server_fd.is_valid()) {
        throw std::runtime_error("Failed to create socket.");
    }

    int opt = 1;
    if (setsockopt(server_fd.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to set socket options.");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd.get(), (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Failed to bind to port " + std::to_string(port) + ".");
    }

    if (::listen(server_fd.get(), 10) < 0) {
        throw std::runtime_error("Failed to listen.");
    }

    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        int addrlen = sizeof(address);
        int client_fd = accept(server_fd.get(), (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection.\n";
            continue;
        }

        try {
            thread_pool->enqueue([this, client_fd] {
                this->handle_client(client_fd);
            });
        } catch (const std::exception& e) {
            std::cerr << "Failed to enqueue task: " << e.what() << "\n";
            ::close(client_fd);
        }
    }
}


void Server::handle_client(int client_fd_raw) {
    ScopedFD client_fd(client_fd_raw);

    char buffer[8192] = {0};
    ssize_t bytes_read = read(client_fd.get(), buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        return; // client_fd gets automatically closed by ScopedFD
    }

    try {
        Request req = utils::parse_request(std::string(buffer, bytes_read));
        Response res;

        router->route(req, res);
        send_response(client_fd.get(), res);
    } catch (const std::exception& e) {
        std::cerr << "Exception in request handling: " << e.what() << "\n";
        Response res;
        res.status_code = 500;
        res.body = "500 Internal Server Error";
        res.content_type = "text/plain";
        send_response(client_fd.get(), res);
    } catch (...) {
        std::cerr << "Unknown exception in request handling.\n";
        Response res;
        res.status_code = 500;
        res.body = "500 Internal Server Error";
        res.content_type = "text/plain";
        send_response(client_fd.get(), res);
    }
}


void Server::send_response(int client_fd, const Response& res) {
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << res.status_code << " " << utils::get_status_message(res.status_code) << "\r\n"
                    << "Content-Type: " << res.content_type << "\r\n"
                    << "Content-Length: " << res.body.length() << "\r\n"
                    << "Connection: close\r\n";

    for (const auto& [key, value] : res.headers) {
        response_stream << key << ": " << value << "\r\n";
    }

    response_stream << "\r\n" << res.body;

    std::string response_str = response_stream.str();
    send(client_fd, response_str.c_str(), response_str.length(), MSG_NOSIGNAL);
}

} // namespace cppweb
