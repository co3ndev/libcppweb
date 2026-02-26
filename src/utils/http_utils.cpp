#include "../../include/cppweb/utils/http_utils.hpp"
#include <sstream>
#include <algorithm>

namespace cppweb::utils {

std::string get_status_message(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 503: return "Service Unavailable";
        default:  return "Unknown";
    }
}

std::string get_mime_type(const std::string& ext) {
    std::string lower_ext = ext;
    std::transform(lower_ext.begin(), lower_ext.end(), lower_ext.begin(), ::tolower);

    if (lower_ext == ".html" || lower_ext == ".htm") return "text/html";
    if (lower_ext == ".css") return "text/css";
    if (lower_ext == ".js") return "application/javascript";
    if (lower_ext == ".json") return "application/json";
    if (lower_ext == ".xml") return "application/xml";
    if (lower_ext == ".txt") return "text/plain";
    if (lower_ext == ".csv") return "text/csv";
    if (lower_ext == ".png") return "image/png";
    if (lower_ext == ".jpg" || lower_ext == ".jpeg") return "image/jpeg";
    if (lower_ext == ".gif") return "image/gif";
    if (lower_ext == ".svg") return "image/svg+xml";
    if (lower_ext == ".ico") return "image/x-icon";
    if (lower_ext == ".webp") return "image/webp";
    if (lower_ext == ".mp4") return "video/mp4";
    if (lower_ext == ".webm") return "video/webm";
    if (lower_ext == ".mp3") return "audio/mpeg";
    if (lower_ext == ".wav") return "audio/wav";
    if (lower_ext == ".pdf") return "application/pdf";
    if (lower_ext == ".zip") return "application/zip";
    if (lower_ext == ".tar") return "application/x-tar";
    if (lower_ext == ".gz") return "application/gzip";

    return "application/octet-stream";
}

Request parse_request(const std::string& raw_data) {
    std::istringstream request_stream(raw_data);
    std::string method, raw_path, http_version;
    request_stream >> method >> raw_path >> http_version;

    // Consume the trailing newline from the first line
    std::string line;
    std::getline(request_stream, line);

    // Extract query parameters from raw_path
    std::string path = raw_path;
    std::map<std::string, std::string> query_params;
    size_t query_pos = raw_path.find('?');

    if (query_pos != std::string::npos) {
        path = raw_path.substr(0, query_pos);
        std::string query_string = raw_path.substr(query_pos + 1);

        // Split query string by '&'
        std::istringstream query_stream(query_string);
        std::string key_value;
        while (std::getline(query_stream, key_value, '&')) {
            size_t eq_pos = key_value.find('=');
            if (eq_pos != std::string::npos) {
                query_params[key_value.substr(0, eq_pos)] = key_value.substr(eq_pos + 1);
            } else if (!key_value.empty()) {
                query_params[key_value] = "";
            }
        }
    }

    Request req{method, path, "", {}, query_params};

    // Header Parsing
    while (std::getline(request_stream, line) && line != "\r" && !line.empty()) {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);

            // Skip leading whitespace in the value
            size_t value_start = line.find_first_not_of(" \t", colon_pos + 1);
            std::string value = "";
            if (value_start != std::string::npos) {
                value = line.substr(value_start);
                // Clean up trailing '\r' typical in HTTP headers
                if (!value.empty() && value.back() == '\r') {
                    value.pop_back();
                }
            }
            req.headers[key] = value;
        }
    }

    // The remaining data in the stream is the body
    std::ostringstream body_stream;
    body_stream << request_stream.rdbuf();
    req.body = body_stream.str();

    return req;
}

} // namespace cppweb::utils
