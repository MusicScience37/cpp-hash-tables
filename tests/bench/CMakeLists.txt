option(HASH_TABLES_TEST_BENCHMARKS "execute benchmarks in tests" OFF)

set(HASH_TABLES_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are outputed" FORCE)
file(MAKE_DIRECTORY ${HASH_TABLES_BENCH_DIR})

function(target_add_to_benchmark TARGET)
    target_link_libraries(${TARGET} PRIVATE cpp_stat_bench::cpp_stat_bench
                                            hash_tables hash_tables_test_helper)
    target_include_directories(${TARGET}
                               PRIVATE ${HASH_TABLES_BENCH_INCLUDE_DIR})

    if(HASH_TABLES_TEST_BENCHMARKS)
        add_test(
            NAME ${TARGET}
            COMMAND ${TARGET} --plot "${TARGET}/" --json "${TARGET}/result.json"
            WORKING_DIRECTORY ${HASH_TABLES_BENCH_DIR})
    endif()
endfunction()

add_subdirectory(std)
add_subdirectory(utility)
add_subdirectory(tables)
add_subdirectory(maps)
