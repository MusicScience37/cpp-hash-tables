cmake_minimum_required(VERSION 3.12)

project(hash_tables_test LANGUAGES CXX)
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD
    17
    CACHE STRING "C++ standard")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# for find_package
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

find_package(cpp_hash_tables REQUIRED)

enable_testing()
add_executable(example example.cpp)
target_link_libraries(example cpp_hash_tables::cpp_hash_tables)
add_test(
    NAME example
    COMMAND example
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
