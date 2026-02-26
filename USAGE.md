### libcppweb usage

So you want to build a web server in C++? We've got you covered. Here is how you use `libcppweb`.

## The Basics
Include the header, create a server, add some routes, and tell it to listen. It's that easy.

```cpp
#include "cppweb.hpp"

int main() {
    cppweb::Server app;

    app.get("/", [](const cppweb::Request& req, cppweb::Response& res) {
        res.body = "Hello from libcppweb!";
    });

    app.listen(8080);
    return 0;
}
```

## Routing
You can handle `GET` and `POST` requests. Just pass a lambda!

```cpp
// Return some JSON
app.get("/api/status", [](const cppweb::Request& req, cppweb::Response& res) {
    res.body = R"({"status": "ok"})";
    res.content_type = "application/json";
});

// Handle a POST body
app.post("/api/echo", [](const cppweb::Request& req, cppweb::Response& res) {
    res.body = req.body; // echo the raw body back
});
```

Want to just serve an HTML file on a specific route? You don't even need a lambda:
```cpp
app.get("/test", "/path/to/example.html");
```

## Static Files
Got a whole folder of images, scripts, or stylesheets? Serve them all at once.
```cpp
app.serve_static("/static", "/path/to/your/folder/");
```
Now, hitting `/static/style.css` in your browser will serve `style.css` from that folder.

## The Data: Requests & Responses
Your lambda always gets a `cppweb::Request` and a `cppweb::Response`.

**The Request (`req`) gives you what the client sent:**
* `req.method`: The HTTP method ("GET", "POST", etc.)
* `req.path`: The requested URL path.
* `req.body`: The raw string payload of the request.
* `req.headers`: A map of the HTTP headers.
* `req.query_params`: A map of the parsed URL queries (e.g., `?key=value`).

**The Response (`res`) is what you send back:**
* `res.status_code`: Defaults to `200`. Change it to `404` or `500` if things go south.
* `res.body`: The actual text, HTML, or JSON you want to send.
* `res.content_type`: Defaults to `"text/plain"`. Change it to `"text/html"` or `"application/json"` as needed.