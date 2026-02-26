#include "../include/cppweb.hpp"

int main() {
    cppweb::Server app;

    // Define a GET route
    app.get("/", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = "Hello from libcppweb!";
        res.content_type = "text/plain";
    });

    // Define a specific file route
    app.get("/test", "/home/michael/Projects/libcweb/tests/example.html");

    // Define a JSON API route
    app.get("/api/status", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = R"({"status": "ok", "version": "0.1.0"})";
        res.content_type = "application/json";
    });

    // Define a POST route to test body extraction
    app.post("/api/echo", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = R"({"status": "ok"})";
        res.content_type = "application/json";
    });

    // Test for static routes: visit /static
    app.serve_static("/static", "/home/michael/Projects/libcweb/tests/");

    // Start server on port 8080
    app.listen(8080);

    return 0;
}
