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
    void terminal_messages()
    {
        int buffer_size = 240;
        for(int i = 0; i < buffer_size; i++)
        {
            // its looks like a mess but its to ensure that all messages are consumed without blocking
            // later we can optimize this by using condition variables or other signaling mechanisms
            if (!packageManager.errors.empty())
            {
                terminal.AddErrorMessage(packageManager.errors.front());
                packageManager.errors.pop();
            }
            if (!packageManager.warnings.empty())
            {
                terminal.AddWarningMessage(packageManager.warnings.front());
                packageManager.warnings.pop();
            }
            if (!packageManager.outputs.empty())
            {
                terminal.AddOutputMessage(packageManager.outputs.front());
                packageManager.outputs.pop();
            }
            if(!import_graph_panel.errors.empty())
            {
                terminal.AddErrorMessage(import_graph_panel.errors.front());
                import_graph_panel.errors.pop();
            }
            if(!import_graph_panel.warnings.empty())
            {
                terminal.AddWarningMessage(import_graph_panel.warnings.front());
                import_graph_panel.warnings.pop();
            }
            if(!import_graph_panel.outputs.empty())
            {
                terminal.AddOutputMessage(import_graph_panel.outputs.front());
                import_graph_panel.outputs.pop();
            }
        }
        
    }

public:
    PanelManager() : terminal(600, 400, "Terminal"), import_panel(600, 400, "Import"), import_graph_panel(600, 400, "Import Graph","Import"), packageManager(pythonruntime), runtime_graph_panel(600, 400, "Runtime","Runtime") {}
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
        terminal_messages();
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