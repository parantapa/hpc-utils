# type: ignore
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain, CMake


class HpcUtilsRecipe(ConanFile):
    name = "hpc-utils"
    version = "0.1.0"

    license = "MIT"
    author = "Parantapa Bhattacharya <pb@parantapa.net>"
    url = "https://github.com/parantapa/hpc-utils"
    description = "Utility code to simplify development of HPC programs."

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("fmt/12.1.0")
        self.requires("parallel-hashmap/2.0.0")
        self.requires("random123/1.14.0")
        self.requires(
            "arrow/22.0.0",
            options=dict(
                with_csv=True,
                with_lz4=True,
                with_snappy=True,
                with_zstd=True,
            ),
        )
        self.requires("hdf5/1.14.6")

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.user_presets_path = False
        toolchain.generate()

        cmake = CMakeDeps(self)
        cmake.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs = ["lib/cmake/hpc-utils"]
