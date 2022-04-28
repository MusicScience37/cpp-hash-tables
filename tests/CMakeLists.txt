option(HASH_TABLES_WRITE_JUNIT
       "write JUnit XML files in tests of ${FULL_PROJECT_NAME}" OFF)
if(HASH_TABLES_WRITE_JUNIT)
    set(HASH_TABLES_JUNIT_DIR
        "${CMAKE_BINARY_DIR}/junit"
        CACHE PATH "directory to which JUnit XML files are outputed" FORCE)
    file(MAKE_DIRECTORY ${HASH_TABLES_JUNIT_DIR})
endif()

set(HASH_TABLES_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${HASH_TABLES_TEMP_TEST_DIR})

add_library(hash_tables_test_helper INTERFACE)
target_include_directories(hash_tables_test_helper
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

macro(target_add_catch2 TARGET)
    target_link_libraries(
        ${TARGET} PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
                          hash_tables hash_tables_test_helper)

    if(HASH_TABLES_WRITE_JUNIT)
        set(CATCH2_REPORT_OPTIONS -r junit -o
                                  ${HASH_TABLES_JUNIT_DIR}/${TARGET}.xml)
    else()
        set(CATCH2_REPORT_OPTIONS -r console)
    endif()

    add_test(
        NAME ${TARGET}
        COMMAND ${TARGET} ${CATCH2_REPORT_OPTIONS}
        WORKING_DIRECTORY ${HASH_TABLES_TEMP_TEST_DIR})
    target_add_ausan(${TARGET})
endmacro()

add_subdirectory(units)

option(HASH_TABLES_ENABLE_BENCH "enable benchmarks of ${FULL_PROJECT_NAME}" OFF)
if(HASH_TABLES_ENABLE_BENCH)
    # add_subdirectory(bench)
endif()
