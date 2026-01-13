#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <print>

// Centralized error handler for all GLFW issues.
static void glfwErrorCallback(int error, const char* description) {
    std::println(stderr, "GLFW Error {}: {}", error, description);
}


int main(int, char**) {

    std.println("Argus Engine: Initializing Graphics Subsystem...");

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
    if(glewInit() !=  GLEW_OK) {
        std::println(stderr, "Failed to initialize OpenGL loader!");
        return 1;
    }

    // --- ImGui Setup ---

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
    ImGui_ImplOpenGL3_Init("#version 130"); // Specify GLSL version.

    // --- Main Application Loop ---
    std::println("Argus Engine: Online. Entering Main Loop");
    while(!glfwWindowShouldClose(window)) {
        // Check for user input events (e.g., keyboard, mouse).
        glfwPollEvents();

        // Signal the start of a new ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- UI Definition ---
        // All ImGui widget calls must happen between NewFrame() and Render().
        {
            ImGui::Begin("Argus Debug Console");

            ImGui::Text("System Status: OPERATIONAL");
            ImGui::Separator();

            if(ImGui::Button("Test Connection")) {
                std::println("Button Clicked: Testing...");
            }

            // Display real-time performance metrics.
            ImGui::Text("Performance: %.3f ms/frame (%.1f FPS)",
            1000.0f / io.Framerate, io.Framerate);

            ImGui::End();
        }

        // --- Rendering ---
        // Finalize the ImGui draw data.
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

    // --- Shutdown & Cleanup ---
    // Orderly shutdown is crucial to avoid resource leaks.
    // Shut down ImGui backends first, then destroy its context.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Finally, terminate GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
