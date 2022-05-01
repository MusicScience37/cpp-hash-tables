#include <stdexcept>

#include <hash_tables/maps/open_address_map_st.h>

auto main() -> int {  // NOLINT
    hash_tables::maps::open_address_map_st<int, int> map;
    map.emplace(1, 2);
    if (map.at(1) != 2) {
        throw std::runtime_error("Assertion failed.");
    }
    return 0;
}
