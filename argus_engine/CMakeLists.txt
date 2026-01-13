cmake_minimum_required(VERSION 3.25)
project(ArgusEngine LANGUAGES CXX)

# Require C++23 to leverage modern language features.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate a compile_commands.json for tooling support (e.g., clangd).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Dependency Resolution ---
# Conan is used for managing most dependencies.
# These are located and linked via find_package.
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(FFMPEG REQUIRED)

# Use FetchContent for ImGui to ensure a specific, consistent version
# is used directly from its repository, avoiding potential version conflicts
# with a system-installed or package-manager-installed library.
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

# --- ImGui Configuration ---
# Create a dedicated static library for ImGui. This isolates its build
# and clearly defines the interface for the main application.
add_library(imgui_lib STATIC
    # Core ImGui files
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    # Backend "glue" code for GLFW and OpenGL3.
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Expose ImGui's headers to any target that links against imgui_lib.
target_include_directories(imgui_lib PUBLIC 
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

# Link ImGui to its own dependencies. This ensures that any executable
# linking against imgui_lib will also correctly link against these.
target_link_libraries(imgui_lib PUBLIC glfw GLEW::GLEW)
if(WIN32)
    # OpenGL is a system library on Windows.
    target_link_libraries(imgui_lib PUBLIC opengl32)
endif()

# --- Main Application ---
set(SOURCES
    src/main.cpp 
)

add_executable(ArgusEngine ${SOURCES})

# Link the main executable against all required libraries.
target_link_libraries(ArgusEngine PRIVATE 
    imgui_lib # Our compiled ImGui
    # Dependencies from Conan
    glfw
    GLEW::GLEW
    ffmpeg::avcodec
    ffmpeg::avformat
    ffmpeg::avutil
    ffmpeg::swscale
)

if(WIN32)
    target_link_libraries(ArgusEngine PRIVATE opengl32)
endif()


# --- Testing ---
# Standard setup for enabling CTest.
enable_testing()

# Find the Google Test framework dependency.
find_package(GTest REQUIRED)

# Define the test executable with its source files.
add_executable(ArgusTests tests/test_main.cpp)

# Link the test runner against GTest.
target_link_libraries(ArgusTests PRIVATE GTest::gtest GTest::gtest_main)

# Use the GoogleTest module to automatically discover tests from the source.
include(GoogleTest)
gtest_discover_tests(ArgusTests)