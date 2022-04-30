cmake_minimum_required(VERSION 3.12)

if(DEFINED PROJECT_NAME)
    set(HASH_TABLES_SUBPROJECT ON)
else()
    set(HASH_TABLES_SUBPROJECT OFF)
endif()

project(
    hash_tables
    VERSION 0.0.0
    DESCRIPTION "Hash tables in C++."
    LANGUAGES CXX)
set(FULL_PROJECT_NAME "cpp-hash-tables")
message(STATUS "cpp-hash-tables version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
include(CTest)

option(HASH_TABLES_BUILD_DOC "build documentation of cpp-hash-tables" OFF)
option(HASH_TABLES_TESTING "enable tests of cpp-hash-tables" OFF)
option(HASH_TABLES_BUILD_EXAMPLES "build examples of cpp-hash-tables" OFF)

set(CMAKE_CXX_STANDARD
    17
    CACHE STRING "C++ standard")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CONAN_COMPILER ${CONAN_SETTINGS_COMPILER})
set(CONAN_COMPILER_VERSION ${CONAN_SETTINGS_COMPILER_VERSION})
set(CONAN_LIBCXX ${CONAN_SETTINGS_COMPILER_LIBCXX})
conan_basic_setup()

# find packages installed with Conan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
if(HASH_TABLES_TESTING)
    find_package(Catch2 REQUIRED)
    find_package(trompeloeil REQUIRED)
    find_package(cpp_stat_bench REQUIRED)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ConfigureClangTidy)
include(CppWarningFlags)
include(ConfigureSanitizer)
include(pipenv)

set(HASH_TABLES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(hash_tables INTERFACE)
target_include_directories(hash_tables
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(HASH_TABLES_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(HASH_TABLES_TESTING)
    add_subdirectory(tests)
endif()

if(HASH_TABLES_BUILD_EXAMPLES)
    # add_subdirectory(examples)
endif()
