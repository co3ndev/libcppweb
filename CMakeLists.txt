cmake_minimum_required(VERSION 3.10)

project(libcppweb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Source files for the library
set(CPPWEB_SOURCES
    src/core/server.cpp
    src/routing/router.cpp
    src/threading/thread_pool.cpp
    src/utils/http_utils.cpp
)

# Create the library
add_library(cppweb ${CPPWEB_SOURCES})

# Include directories
target_include_directories(cppweb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link pthread for threading support
find_package(Threads REQUIRED)
target_link_libraries(cppweb PUBLIC Threads::Threads)

# Example application
add_executable(example_app tests/main.cpp)
target_link_libraries(example_app PRIVATE cppweb)

# Optional: Enable testing
enable_testing()

# Add any tests here as needed
# add_executable(test_server tests/test_server.cpp)
# target_link_libraries(test_server PRIVATE cppweb)
# add_test(NAME ServerTests COMMAND test_server)
