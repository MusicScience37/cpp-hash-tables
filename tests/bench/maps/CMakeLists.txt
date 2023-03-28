add_executable(
    hash_tables_bench_maps
    create_pairs.cpp create_pairs_no_reserve.cpp
    create_delete_pairs_concurrent.cpp find_pairs.cpp find_pairs_concurrent.cpp)
target_add_to_benchmark(hash_tables_bench_maps)
