from conans import ConanFile, CMake


class CppHashTablesConan(ConanFile):
    name = "cpp-hash-tables"
    version = "0.0.0"
    description = "Hash tables in C++."
    homepage = "https://gitlab.com/MusicScience37/cpp-hash-tables"
    url = "https://gitlab.com/MusicScience37/cpp-hash-tables.git"
    license = "Apache-2.0"
    author = "Kenta Kabashima (kenta_program37@hotmail.co.jp)"
    topics = ()
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "requirements_for_tests": [True, False],
    }
    default_options = {
        "requirements_for_tests": False,
    }
    exports_sources = ("include/*",)
    no_copy_source = True
    generators = "cmake", "cmake_find_package"

    def requirements(self):
        self.requires("fmt/8.1.1")

    def build_requirements(self):
        if self.options.requirements_for_tests:
            self.build_requires(
                "catch2/3.0.0pre4@MusicScience37+conan-extra-packages/stable"
            )
            self.build_requires("trompeloeil/42")

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
