# How to Use

## Simply Including Headers

This library is header-only,
so you can simply include headers
setting `include` directory to an include directory of compilers.
This library depends only on C++ standard library,
so no further installation is required.

## Via Conan

This library is packaged with [Conan](https://conan.io/),
and available via
[package registry in GitLab](https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-hash-tables/-/packages).

To use this library,
add the package
`cpp_hash_tables/<version>@MusicScience37/stable`
with a version you want
to your `conanfile.py` or `conanfile.txt`,
and add the remote
`https://gitlab.com/api/v4/projects/35726343/packages/conan`
to conan command.
