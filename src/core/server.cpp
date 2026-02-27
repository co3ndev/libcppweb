#include "../../include/cppweb/core/server.hpp"
#include "../../include/cppweb/utils/http_utils.hpp"
#include "../../include/cppweb/utils/codes.hpp"
#include "../../include/cppweb/utils/mime_type.hpp"
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
            // Set file path for streaming instead of reading into memory
            res.file_path = fp.string();
            res.content_type = utils::get_mime_type(fp.extension().string());
            res.status_code = 200;
            return;
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

    std::string raw_request;
    char buffer[8192];
    ssize_t bytes_read = read(client_fd.get(), buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        return; // client_fd gets automatically closed by ScopedFD
    }
    raw_request.append(buffer, bytes_read);

    // Look for HTTP headers end to see if we have a body
    size_t header_end = raw_request.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        size_t cl_pos = raw_request.find("Content-Length:");
        if (cl_pos == std::string::npos) {
            cl_pos = raw_request.find("content-length:");
        }

        if (cl_pos != std::string::npos && cl_pos < header_end) {
            size_t cl_end = raw_request.find("\r\n", cl_pos);
            if (cl_end != std::string::npos) {
                try {
                    std::string cl_str = raw_request.substr(cl_pos + 15, cl_end - cl_pos - 15);
                    // Trim whitespace
                    cl_str.erase(0, cl_str.find_first_not_of(" \t"));
                    cl_str.erase(cl_str.find_last_not_of(" \t") + 1);

                    size_t content_length = std::stoull(cl_str);
                    size_t body_received = raw_request.length() - (header_end + 4);

                    // Read the rest of the payload
                    while (body_received < content_length) {
                        bytes_read = read(client_fd.get(), buffer, sizeof(buffer));
                        if (bytes_read <= 0) break;
                        raw_request.append(buffer, bytes_read);
                        body_received += bytes_read;
                    }
                } catch (...) {
                    // Ignore content-length parsing errors
                }
            }
        }
    }

    try {
        Request req = utils::parse_request(raw_request);
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

    size_t content_length = res.body.length();
    if (!res.file_path.empty()) {
        try {
            content_length = std::filesystem::file_size(res.file_path);
        } catch (const std::exception& e) {
            std::cerr << "Failed to get file size for " << res.file_path << ": " << e.what() << "\n";
            std::string error_response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            send(client_fd, error_response.c_str(), error_response.length(), MSG_NOSIGNAL);
            return;
        }
    }

    response_stream << "HTTP/1.1 " << res.status_code << " " << utils::get_status_message(res.status_code) << "\r\n"
                    << "Content-Type: " << res.content_type << "\r\n"
                    << "Content-Length: " << content_length << "\r\n"
                    << "Connection: close\r\n";

    for (const auto& [key, value] : res.headers) {
        response_stream << key << ": " << value << "\r\n";
    }

    response_stream << "\r\n";

    if (res.file_path.empty()) {
        // Send in-memory body
        response_stream << res.body;
        std::string response_str = response_stream.str();
        send(client_fd, response_str.c_str(), response_str.length(), MSG_NOSIGNAL);
    } else {
        // Send headers first
        std::string headers_str = response_stream.str();
        send(client_fd, headers_str.c_str(), headers_str.length(), MSG_NOSIGNAL);

        // Stream file content
        std::ifstream file(res.file_path, std::ios::binary);
        if (file) {
            char buffer[8192];
            while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
                ssize_t bytes_to_send = file.gcount();
                const char* ptr = buffer;
                while (bytes_to_send > 0) {
                    ssize_t sent = send(client_fd, ptr, bytes_to_send, MSG_NOSIGNAL);
                    if (sent <= 0) return; // Client disconnected or error
                    ptr += sent;
                    bytes_to_send -= sent;
                }
            }
        }
    }
}


} // namespace cppweb
