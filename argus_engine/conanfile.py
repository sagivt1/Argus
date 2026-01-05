from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ArgusEngine(ConanFile):

    name = "argus_engine"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self):
        # Standardize build/source directory layout
        cmake_layout(self)

    def requirements(self):
        # Fast, safe formatting library
        #self.requires("fmt/12.1.0")
        None