add_executable(
    hash_tables_bench_tables
    create_pairs.cpp
    create_pairs_no_reserve.cpp
    create_pairs_multi_tables.cpp
    create_delete_pairs_concurrent.cpp
    find_pairs.cpp
    find_pairs_concurrent.cpp
    check_existence.cpp)
target_add_to_benchmark(hash_tables_bench_tables)
