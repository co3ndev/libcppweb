#include "../include/cppweb.hpp"
#include <iostream>

int main() {
    cppweb::Server app;

    // Define a GET route
    app.get("/", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = "Hello from libcppweb!";
        res.content_type = "text/plain";
    });

    // Define a JSON API route
    app.get("/api/status", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = R"({"status": "ok", "version": "0.1.0"})";
        res.content_type = "application/json";
    });

    // Start server on port 8080
    app.listen(8080);

    return 0;
}