# How to Use

## Simply Including Headers

This library is header-only,
so you can simply include headers
setting `include` directory to an include directory of compilers.
This library depends only on C++ standard library,
so no further installation is required.

## Via vcpkg

This library can be installed via vcpkg using following configurations:

- Add a vcpkg registry
  `https://gitlab.com/MusicScience37Projects/vcpkg-registry`
  in `vcpkg-configuration.json`.

  Example:

  ```json
  {
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg-configuration.schema.json",
    "default-registry": {
      "kind": "git",
      "repository": "https://github.com/Microsoft/vcpkg",
      "baseline": "76b55aaf7c45d05fa516a1f6a69f46fb08b1e720"
    },
    "registries": [
      {
        "kind": "git",
        "repository": "https://gitlab.com/MusicScience37Projects/vcpkg-registry",
        "baseline": "70dffd57a5edbf1a48714c8b0a5a07751d0db298",
        "packages": ["cpp-hash-tables"]
      }
    ]
  }
  ```

- Add `cpp-hash-tables` in `vcpkg.json`

  Example:

  ```json
  {
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
    "dependencies": ["cpp-hash-tables"]
  }
  ```
