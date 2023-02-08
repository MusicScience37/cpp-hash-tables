from conans import ConanFile


class CppHashTablesConan(ConanFile):
    name = "cpp_hash_tables"
    version = "0.4.1"
    description = "Hash tables in C++."
    homepage = (
        "https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-hash-tables"
    )
    url = "https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-hash-tables.git"
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
        pass

    def build_requirements(self):
        if self.options.requirements_for_tests:
            self.build_requires("catch2/3.3.1")
            self.build_requires("trompeloeil/43")
            self.build_requires("cpp_stat_bench/0.10.0@MusicScience37/stable")

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
