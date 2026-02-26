#pragma once

#include <string>
#include <map>
#include "../core/request.hpp"

namespace cppweb::utils {

/**
 * @brief Get the HTTP status message for a given status code
 * @param code The HTTP status code
 * @return The status message string
 */
std::string get_status_message(int code);

/**
 * @brief Get the MIME type for a file extension
 * @param ext The file extension (e.g., ".html", ".json")
 * @return The MIME type string
 */
std::string get_mime_type(const std::string& ext);

/**
 * @brief Parse an HTTP request from raw data
 * @param raw_data The raw HTTP request string
 * @return The parsed Request object
 */
Request parse_request(const std::string& raw_data);

} // namespace cppweb::utils