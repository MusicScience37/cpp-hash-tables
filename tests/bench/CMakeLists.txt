option(HASH_TABLES_TEST_BENCHMARKS "execute benchmarks in tests" OFF)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(HASH_TABLES_ENABLE_HEAVY_BENCH_DEFAULT OFF)
else()
    set(HASH_TABLES_ENABLE_HEAVY_BENCH_DEFAULT ON)
endif()
option(HASH_TABLES_ENABLE_HEAVY_BENCH "enable heavy benchmarks"
       ${HASH_TABLES_ENABLE_HEAVY_BENCH_DEFAULT})

set(HASH_TABLES_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are written" FORCE)
file(MAKE_DIRECTORY ${HASH_TABLES_BENCH_DIR})

function(target_add_to_benchmark TARGET)
    target_link_libraries(${TARGET} PRIVATE cpp_stat_bench::stat_bench
                                            hash_tables hash_tables_test_helper)
    target_include_directories(${TARGET}
                               PRIVATE ${HASH_TABLES_BENCH_INCLUDE_DIR})
    if(HASH_TABLES_ENABLE_HEAVY_BENCH)
        target_compile_definitions(${TARGET}
                                   PRIVATE HASH_TABLES_ENABLE_HEAVY_BENCH=1)
    endif()

    if(HASH_TABLES_TEST_BENCHMARKS)
        add_test(
            NAME ${TARGET}
            COMMAND ${TARGET} --plot "${TARGET}/" --json "${TARGET}/result.json"
            WORKING_DIRECTORY ${HASH_TABLES_BENCH_DIR})
    endif()
endfunction()

add_subdirectory(std)
add_subdirectory(tables)
add_subdirectory(maps)
add_subdirectory(sets)
