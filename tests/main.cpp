#include "../include/cppweb.hpp"

int main() {
    cppweb::Server app(4);

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
        res.body = R"({"received": ")" + req.body + R"("})";
        res.content_type = "application/json";
    });

    // Define a PUT route
    app.put("/api/update", [](const cppweb::Request& req, cppweb::Response& res) {
        res.status_code = 200;
        res.body = R"({"status": "updated"})";
        res.content_type = "application/json";
    });

    // Define a DELETE route
    app.del("/api/delete/:id", [](const cppweb::Request& req, cppweb::Response& res) {
        res.status_code = 204;
        res.body = "";
    });

    // Start server on port 8080
    app.listen(8080);

    return 0;
}
