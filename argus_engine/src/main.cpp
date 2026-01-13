#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <print>

static void glfwErrorCallback(int error, const char* description) {
    std::println(stderr, "GLFW Error {}: {}", error, description);
}


int main(int, char**) {

    std::println("Argus Engine: Initializing Graphics Subsystem...");

    // setup windown (GLFW)
    glfwSetErrorCallback(glfwErrorCallback);
    if(!glfwInit()) return 1;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Argus: Vision Pulse (ImGui Build)", nullptr, nullptr);
    if (window == nullptr) return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // enable vsync

    // Setup OpenGL loader (GLEW)
    if(glewInit() !=  GLEW_OK) {
        std::println(stderr, "Failed to initialize OpenGL loader!");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style (Dark Mode)
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Main App Loop
    std::println("Argus Engine: Online. Entering Main Loop");
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw Interface
        {
            ImGui::Begin("Argus Debug Console");

            ImGui::Text("System Status: OPERATIONAL");
            ImGui::Separator();

            if(ImGui::Button("Test Connection")) {
                std::println("Button Clicked: Testing...");
            }

            ImGui::Text("Performance: %.3f ms/frame (%.1f FPS)",
            1000.0f / io.Framerate, io.Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
