# Quick Reference

A quick syntax guide for libcppweb.

## Basic Server Setup

```cpp
#include <cppweb.hpp>

int main() {
    cppweb::Server server(4); // Create server with 4 threads
    
    // Register routes here
    
    server.listen(8080); // Start server on port 8080
    return 0;
}
```

## Route Handlers

All route handlers follow this signature:

```cpp
void handler(const cppweb::Request& req, cppweb::Response& res) {
    // Handle request and set response
}
```

## Registering Routes

### GET Routes

```cpp
// Handler function
server.get("/path", [](const cppweb::Request& req, cppweb::Response& res) {
    res.body = "Hello, World!";
    res.status_code = 200;
});

// Serve static file
server.get("/file", "./path/to/file.html");
```

### POST Routes

```cpp
server.post("/submit", [](const cppweb::Request& req, cppweb::Response& res) {
    std::string data = req.body;
    res.body = "Data received";
    res.status_code = 201;
});
```

### PUT Routes

```cpp
server.put("/update/:id", [](const cppweb::Request& req, cppweb::Response& res) {
    std::string data = req.body;
    res.body = "Updated";
    res.status_code = 200;
});
```

### DELETE Routes

```cpp
server.del("/delete/:id", [](const cppweb::Request& req, cppweb::Response& res) {
    res.body = "Deleted";
    res.status_code = 204;
});
```

## Request Object

```cpp
struct Request {
    std::string method;                              // HTTP method (GET, POST, etc.)
    std::string path;                                // URL path
    std::string body;                                // Request body
    std::map<std::string, std::string> headers;      // HTTP headers
    std::map<std::string, std::string> query_params; // Query parameters
};
```

### Accessing Request Data

```cpp
server.get("/example", [](const cppweb::Request& req, cppweb::Response& res) {
    // Get query parameter: /example?id=123
    std::string id = req.query_params["id"];
    
    // Get header
    std::string user_agent = req.headers["User-Agent"];
    
    // Get body (POST/PUT requests)
    std::string body = req.body;
});
```

## Response Object

```cpp
struct Response {
    int status_code = 200;                           // HTTP status code
    std::string body;                                // Response body
    std::string content_type = "text/plain";         // Content-Type header
    std::map<std::string, std::string> headers;      // Custom headers
};
```

### Setting Response Data

```cpp
server.get("/api/data", [](const cppweb::Request& req, cppweb::Response& res) {
    res.status_code = 200;
    res.content_type = "application/json";
    res.body = R"({"status": "ok"})";
    res.headers["X-Custom-Header"] = "value";
});
```

## Common Status Codes

| Code | Meaning |
|------|---------|
| 200 | OK |
| 201 | Created |
| 204 | No Content |
| 400 | Bad Request |
| 404 | Not Found |
| 405 | Method Not Allowed |
| 500 | Internal Server Error |

## Common Content Types

| Type | MIME Type |
|------|-----------|
| JSON | `application/json` |
| HTML | `text/html` |
| Plain Text | `text/plain` |
| CSS | `text/css` |
| JavaScript | `application/javascript` |
| PNG | `image/png` |
| JPEG | `image/jpeg` |

## Example Application

```cpp
#include <cppweb.hpp>

int main() {
    cppweb::Server server(4);
    
    // Simple GET
    server.get("/", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = "Welcome!";
        res.content_type = "text/html";
    });
    
    // GET with query params
    server.get("/greet", [](const cppweb::Request& req, cppweb::Response& res) {
        std::string name = req.query_params["name"];
        res.body = "Hello, " + name + "!";
    });
    
    // POST handler
    server.post("/api/users", [](const cppweb::Request& req, cppweb::Response& res) {
        res.status_code = 201;
        res.body = "User created";
    });
    
    // Serve static file
    server.get("/style.css", "./assets/style.css");
    
    server.listen(8080);
    return 0;
}
```
