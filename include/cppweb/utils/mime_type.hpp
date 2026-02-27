#pragma once

#include <string>

namespace cppweb::utils {

/**
 * @brief Get the MIME type for a file extension
 * @param ext The file extension (e.g., ".html", ".json")
 * @return The MIME type string
 */
std::string get_mime_type(const std::string& ext);

}
