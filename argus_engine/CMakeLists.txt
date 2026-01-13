cmake_minimum_required(VERSION 3.25)
project(ArgusEngine LANGUAGES CXX)

# Enforce Modern C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(FFMPEG REQUIRED)

# 2. Fetch ImGui Directly from GitHub (The Fix)
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.91.5
)
FetchContent_MakeAvailable(imgui)

# 3. Define the ImGui Library with Backends
# We manually tell CMake which files to compile
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    # The Backend Files (Glue Code)
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# Tell the compiler where to find headers
target_include_directories(imgui_lib PUBLIC 
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
)

# Link ImGui to GLFW and OpenGL so it can compile the glue code
target_link_libraries(imgui_lib PUBLIC glfw GLEW::GLEW)
if(WIN32)
    target_link_libraries(imgui_lib PUBLIC opengl32)
endif()

# Source Files
set(SOURCES
    src/main.cpp 
)

# Executable
add_executable(ArgusEngine ${SOURCES})

# Linking
target_link_libraries(ArgusEngine PRIVATE 
    imgui_lib
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


# Testing section
enable_testing()

find_package(GTest REQUIRED)

add_executable(ArgusTests tests/test_main.cpp)

target_link_libraries(ArgusTests PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ArgusTests)