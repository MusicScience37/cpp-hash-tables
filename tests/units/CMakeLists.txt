include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(hash_tables_test_units ${SOURCE_FILES})
target_add_catch2(hash_tables_test_units)
target_add_ausan(hash_tables_test_units)

add_executable(hash_tables_test_units_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(
    hash_tables_test_units_unity PRIVATE Catch2::Catch2WithMain hash_tables
                                         hash_tables_test_helper)
