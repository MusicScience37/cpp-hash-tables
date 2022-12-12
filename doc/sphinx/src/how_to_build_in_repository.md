# How to Build in the Repository

## Dependencies

A development environment including dependencies can be created using
[Devcontainer of VSCode](https://code.visualstudio.com/docs/remote/containers).

To build this repository,
you will require following dependencies:

- [Python](https://www.python.org/) 3.9
  - You may want to use [pyenv](https://github.com/pyenv/pyenv).
- [pipenv](https://pipenv.pypa.io/en/latest/)
  - Required Python packages can be installed using pipenv.
    Execute the command `pipenv install --dev` on this directory.
  - [pre-commit](https://pre-commit.com/)
    will be installed via pipenv, and used for some checks.
    Execute `pipenv run pre-commit install` on this directory
    if you develop this library.
- [CMake](https://cmake.org/)
- C++ 17 Compiler
  - Following compilers are tested in CI:
    - GCC 10
    - GCC 12
    - Clang 14
    - MSVC 19

Additionally, you will require following dependencies
to build documentation.

- [Doxygen](https://www.doxygen.nl/index.html)
- [Graphviz](https://graphviz.org/)
- [PlantUML](https://plantuml.com)
  - Set `PLANTUML_JAR_PATH` environment variable to specify the place of `plantuml.jar` file.
- Java runtime environment (JRE) for PlantUML.

## Build commands

After installation of dependencies,
execute the following commands to build:

1. Start a shell of pipenv.

   ```bash
   cd <this-directory>
   pipenv sync --dev
   pipenv shell
   ```

2. Add a Conan remote in GitLab. (Once in an environment. Already done in devcontainer of VSCode.)

   ```bash
   conan remote add cpp-stat-bench https://gitlab.com/api/v4/projects/32226502/packages/conan
   ```

3. Download and install required Conan packages.

   ```bash
   python3 ./scripts/install_conan_dependencies.py <build_type>
   ```

   `build_type` can be `Debug`, `RelWithDebInfo`, or `Release`.

4. Configure.

   ```bash
   cd build
   cmake ..
   ```

5. Optionally edit options.

   ```bash
   <some_editor> CMakeCache.txt
   ```

6. Build.

   ```bash
   cmake --build .
   ```

7. Optionally run tests.

   ```bash
   ctest
   ```
