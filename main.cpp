#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <thread>
#include "bridge/Package.hpp"
#include "Panel/Panel.hpp"

PythonRuntime pythonruntime;

class PanelManager
{
private:
    Terminal terminal;
    ImportPanel import_panel;
    GraphPanel import_graph_panel;
    GraphPanel runtime_graph_panel;
    Package packageManager;
    bool isLoaded = false;
    bool requested = false;

public:
    PanelManager() : terminal(600, 400, "Terminal"), import_panel(600, 400, "Import"), import_graph_panel(600, 400, "Import Graph"), packageManager(pythonruntime), runtime_graph_panel(600,400, "Runtime"){}
    void run_panels()
    {
        terminal.run();

        if (!requested)
        {
            packageManager.refresh_async();
            requested = true;
        }

        auto pkgs = packageManager.consume_packages();

        if (!pkgs.empty() && !isLoaded)
        {
            import_panel.setPackages(pkgs);
            import_graph_panel.setPackages(pkgs);
            isLoaded = true;
        }

        import_panel.run();
        import_graph_panel.run();
        runtime_graph_panel.run();
        if (import_panel.getUpdate())
        {
            requested = false;
            isLoaded = false;
            import_panel.setUpdate(false);
        }
    }
};

// main program
void main_program()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Vishwakarma", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    PanelManager panelmanager = PanelManager();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);
        panelmanager.run_panels();
        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup);
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

int main()
{
    main_program();
    return 0;
}