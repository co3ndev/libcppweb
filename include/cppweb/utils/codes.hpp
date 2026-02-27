#pragma once

#include <string>

namespace cppweb::utils {

/**
 * @brief Get the HTTP status message for a given status code
 * @param code The HTTP status code
 * @return The status message string
 */
std::string get_status_message(int code);

}
