cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ParseVersion)
parse_version(${CMAKE_CURRENT_SOURCE_DIR}/include/hash_tables/version.h
              HASH_TABLES)

project(
    hash_tables
    VERSION ${HASH_TABLES_VERSION}
    DESCRIPTION "Hash tables in C++."
    LANGUAGES CXX)
set(FULL_PROJECT_NAME "cpp-hash-tables")
message(STATUS "${FULL_PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")

set(BUILDNAME
    "${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}-${CMAKE_BUILD_TYPE}"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake)
include(CTest)

option(HASH_TABLES_BUILD_DOC "build documentation of ${FULL_PROJECT_NAME}" OFF)
option(HASH_TABLES_TESTING "enable tests of ${FULL_PROJECT_NAME}" OFF)
option(HASH_TABLES_BUILD_EXAMPLES "build examples of ${FULL_PROJECT_NAME}" OFF)
option(HASH_TABLES_ENABLE_PROFILING "enable profiling of ${FULL_PROJECT_NAME}"
       OFF)

set(CMAKE_CXX_STANDARD
    17
    CACHE STRING "C++ standard")

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "export compile commands" FORCE)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/lib>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

option(HASH_TABLES_USE_CONAN "Use Conan as the package manager for C++." OFF)
if(HASH_TABLES_USE_CONAN)
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
else()
    # Via vcpkg.
    find_package(Catch2 REQUIRED)
    find_package(trompeloeil REQUIRED)
    find_package(cpp-stat-bench REQUIRED)
endif()

include(CppWarningFlags)
include(ConfigureSanitizer)
include(poetry)

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

if(HASH_TABLES_TESTING AND HASH_TABLES_ENABLE_PROFILING)
    add_subdirectory(profiling)
endif()
