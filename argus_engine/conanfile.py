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
        self.requires("glfw/3.4")
        self.requires("glew/2.2.0")

        self.requires("ffmpeg/7.0.1")

        self.requires("gtest/1.17.0")
    
    def configure(self):
        self.options["imgui"].backend_glfw = True
        self.options["imgui"].backend_opengl3 = True

        self.options["ffmpeg"].shared = True



        