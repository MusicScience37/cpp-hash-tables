add_executable(hash_tables_bench_tables_create_pairs create_pairs.cpp)
target_add_to_benchmark(hash_tables_bench_tables_create_pairs)

add_executable(hash_tables_bench_tables_find_pairs find_pairs.cpp)
target_add_to_benchmark(hash_tables_bench_tables_find_pairs)

add_executable(hash_tables_bench_tables_create_delete_pairs
               create_delete_pairs.cpp)
target_add_to_benchmark(hash_tables_bench_tables_create_delete_pairs)

add_executable(hash_tables_bench_tables_create_delete_pairs_concurrent
               create_delete_pairs_concurrent.cpp)
target_add_to_benchmark(hash_tables_bench_tables_create_delete_pairs_concurrent)

add_executable(hash_tables_bench_tables_find_pairs_concurrent
               find_pairs_concurrent.cpp)
target_add_to_benchmark(hash_tables_bench_tables_find_pairs_concurrent)
