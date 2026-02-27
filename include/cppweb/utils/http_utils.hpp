#pragma once

#include <string>
#include "../core/request.hpp"

namespace cppweb::utils {
/**
 * @brief Parse an HTTP request from raw data
 * @param raw_data The raw HTTP request string
 * @return The parsed Request object
 */
Request parse_request(const std::string& raw_data);

} // namespace cppweb::utils
