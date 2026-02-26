cmake_minimum_required(VERSION 3.10)

project(libcppweb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(cppweb src/cppweb.cpp)

target_include_directories(cppweb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(example_app examples/main.cpp)

target_link_libraries(example_app PRIVATE cppweb)