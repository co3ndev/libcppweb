#include "../../include/cppweb/utils/http_utils.hpp"
#include <sstream>
#include <algorithm>

namespace cppweb::utils {

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
}
