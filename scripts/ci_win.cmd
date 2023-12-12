call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat" x86_x64

cmake .. ^
    -G Ninja ^
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ^
    "-DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DHASH_TABLES_TESTING:BOOL=ON ^
    -DHASH_TABLES_ENABLE_BENCH=ON ^
    -DHASH_TABLES_BUILD_EXAMPLES=ON ^
    -DHASH_TABLES_TEST_EXAMPLES=ON ^
    -DHASH_TABLES_WRITE_JUNIT:BOOL=ON

cmake --build . --config Release --parallel

ctest -V --build-config Release
