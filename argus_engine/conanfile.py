from conan import ConanFile
from conan.tools.cmake import cmake_layout

class ArgusEngine(ConanFile):

    name = "argus_engine"
    version = "0.1.0"
    
    # Defines the matrix of configurations to build for.
    settings = "os", "compiler", "build_type", "arch"
    
    # Generates CMake integration files (toolchain and dependencies).
    generators = "CMakeToolchain", "CMakeDeps"

    def layout(self):
        # Use the standard Conan layout for CMake projects (e.g., build/Debug).
        cmake_layout(self)

    def requirements(self):
        # --- Graphics & Windowing ---
        self.requires("glfw/3.4")
        self.requires("glew/2.2.0")

        # --- Media Processing ---
        self.requires("ffmpeg/7.0.1")

        # --- Testing Framework ---
        self.requires("gtest/1.17.0")
    
    def configure(self):
        # Build FFmpeg as a shared library (DLL/SO) to reduce final binary size.
        self.options["ffmpeg"].shared = True



        