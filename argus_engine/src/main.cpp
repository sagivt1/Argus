#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <print>
#include "SharedMemory.hpp"

// Centralized error handler for all GLFW issues.
static void glfwErrorCallback(int error, const char* description) {
    std::println(stderr, "GLFW Error {}: {}", error, description);
}

// Command-line arguments are not used in this application.
int main(int, char**) {

    std::println("Argus Engine: Initializing Graphics Subsystem...");

    // --- Initialization ---

    // Setup a callback to catch and report errors from GLFW.
    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()) return 1;

    // Create the primary OS window and the OpenGL context.
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Argus: Vision Pulse (ImGui Build)", nullptr, nullptr);
    if (window == nullptr) return 1;

    glfwMakeContextCurrent(window);
    // Enable VSync to cap the framerate to the monitor's refresh rate.
    // This prevents screen tearing and reduces unnecessary CPU/GPU usage.
    glfwSwapInterval(1);

    // Initialize the OpenGL function loader (GLEW).
    // This must be done *after* a valid OpenGL context is made current.
    if(glewInit() !=  GLEW_OK) {
        std::println(stderr, "Failed to initialize OpenGL loader!");
        return 1;
    }

    // --- ImGui Setup ---

    // Guards against mismatched header/library versions of ImGui.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // Allow user navigation within ImGui using the keyboard.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Set a default visual theme.
    ImGui::StyleColorsDark();

    // Initialize the platform and renderer backends for ImGui.
    // This "glues" ImGui to our window (GLFW) and our renderer (OpenGL).
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Specify GLSL version for the shader program.

    // --- Synapse Setup ---
    // Establishes an inter-process communication (IPC) channel via shared memory.

    // create a 1MB buffer named "ArgusShm"
    size_t const MEM_SIZE = 1024 * 1024; // 1 megabyte
    SharedMemoryWriter writer("ArgusShm", MEM_SIZE);
    auto buffer = writer.getBuffer();

    // Used to write continuously changing data into the shared buffer.
    unsigned char counter = 0;

    // --- Main Application Loop ---
    std::println("Argus Engine: Online. Entering Main Loop");
    while(!glfwWindowShouldClose(window)) {
        // Check for user input events (e.g., keyboard, mouse).
        glfwPollEvents();

        // --- Data Production ---
        // Simulates a live data feed by writing a new value to a portion
        // of the shared memory buffer on each frame.
        std::fill(buffer.begin(), buffer.begin() + 100, counter);
        counter++;

        // --- UI Definition & Rendering ---
        // All ImGui and rendering calls are grouped in this block.
        {
            // Signal the start of a new ImGui frame.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Argus Synapse Status");
            ImGui::Text("Shared Memory: ACTIVE");
            ImGui::Text("Signal Value: %d", counter); // Display the value being written.
            ImGui::End();

            // --- Rendering ---
            // Compiles the UI description into a set of draw commands.
            ImGui::Render();

            // Get the actual framebuffer size to handle high-DPI displays.
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);

            // Clear the screen with a solid color.
            glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Execute the ImGui draw commands.
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Swap the front and back buffers to display the rendered frame.
            glfwSwapBuffers(window);
        }
    }

    // --- Shutdown & Cleanup ---
    // Orderly shutdown is crucial to avoid resource leaks.
    // The SharedMemoryWriter's destructor will automatically handle shared memory removal.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Finally, terminate GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
