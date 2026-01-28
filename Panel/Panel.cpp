#include "Panel.hpp"

// Base window runner to centralize ImGui Begin/End logic.
void Windows::run()
{
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
    ImGui::Begin(title.c_str());
    addComponent();
    ImGui::End();
}

// Terminal class implementation
Terminal::Terminal(int width, int height, const std::string title) : Windows(width, height, title) {}

void Terminal::exec_cmd(const std::string &cmd)
{
    std::array<char, 256> buffer;
    std::string result;

    FILE *pipe = _popen(cmd.c_str(), "r");
    if (!pipe)
    {
        Terminal::command_history.push_back("Failed to run command");
        return;
    };

    while (fgets(buffer.data(), buffer.size(), pipe))
        Terminal::command_history.push_back(buffer.data());

    _pclose(pipe);
}

void Terminal::run_pip_async(const std::string &cmd)
{
    std::thread([this, cmd]()
                { Terminal::exec_cmd("python -m " + cmd); })
        .detach();
}

void Terminal::addComponent()
{
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto &line : Terminal::command_history)
    {
        ImGui::TextUnformatted(line.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    if (ImGui::InputText("##terminal", Terminal::command, IM_ARRAYSIZE(Terminal::command), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        std::string cmd = Terminal::command;
        Terminal::command_history.push_back("> " + cmd);

        if (cmd == "clear")
        {
            Terminal::command_history.clear();
        }
        else if (cmd == "help")
        {
            Terminal::command_history.push_back("Commands: help, clear, pkg, pip <args>");
        }
        else if (cmd == "pkg")
        {
            Terminal::command_history.push_back("Use Packages panel");
        }
        else if (cmd.rfind("pip ", 0) == 0)
        {
            Terminal::command_history.push_back("Running pip command asynchronously...");
            Terminal::run_pip_async(cmd);
        }
        else
        {
            Terminal::command_history.push_back("Unknown command: " + cmd);
        }

        Terminal::command[0] = '\0';
    }
    ImGui::EndChild();
    ImGui::Separator();
}

// ImportPanel class implementation
ImportPanel::ImportPanel(int width, int height, const std::string title) : Windows(width, height, title) {}

void ImportPanel::addComponent()
{
    if (ImGui::Button("Refresh Packages"))
    {
        force_update = true;
    }
    if (packages.empty())
    {
        ImGui::TextDisabled("No packages loaded.");
    }
    else
    {
        static int selected_pkg = -1;

        if (packages.empty())
        {
            ImGui::TextDisabled("No packages loaded.");
        }
        else
        {
            ImGui::Text("Installed packages:");
            ImGui::Separator();

            ImGui::BeginChild("PackageList", ImVec2(0, 300), true);

            for (int i = 0; i < packages.size(); i++)
            {
                if (ImGui::Selectable(packages[i].c_str(), selected_pkg == i))
                {
                    selected_pkg = i;
                }
            }

            ImGui::EndChild();
        }
    }
}

void ImportPanel::setPackages(const std::vector<std::string> &pkgs)
{
    packages = pkgs;
}

std::vector<std::string> ImportPanel::getPackages() const
{
    return packages;
}

bool ImportPanel::getUpdate()
{
    return force_update;
}

void ImportPanel::setUpdate(bool fu)
{
    force_update = fu;
}

// Node implementation
Node::Node(std::string node, ImVec2 size) : node_name(node), size(size) {}

void Node::SpawnNode(ImDrawList *draw_list, ImVec2 origin, ImVec2 pan_offset)
{
    ImVec2 p1 = ImVec2(origin.x + pan_offset.x + 100, origin.y + pan_offset.y + 100);
    ImVec2 p2 = ImVec2(p1.x + this->size.x, p1.y + this->size.y);
    // printf("Spawning node: %s\n", node_name.c_str());
    draw_list->AddRectFilled(p1, p2, IM_COL32(100, 100, 250, 255));
    draw_list->AddText(ImVec2(110, 110), IM_COL32(255, 255, 255, 255), node_name.c_str());
    // printf("Node %s spawned.\n", node_name.c_str());
}

// Graph class implementation
GraphPanel::GraphPanel(int width, int height, const std::string title) : Windows(width, height, title) {}

void GraphPanel::addComponent()
{
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("GraphCanvas", canvas_size, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("GraphContextMenu");
    }
    GraphPanel::GraphContextMenu();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetCursorScreenPos();

    draw_list->AddRectFilled(origin, ImVec2(origin.x + canvas_size.x, origin.y + canvas_size.y), IM_COL32(50, 50, 50, 255));
    const float GRID_SIZE = 40.0f;
    for (float x = fmodf(pan_offset.x, GRID_SIZE); x < canvas_size.x; x += GRID_SIZE)
    {
        draw_list->AddLine(ImVec2(origin.x + x, origin.y), ImVec2(origin.x + x, origin.y + canvas_size.y), IM_COL32(60, 60, 60, 255));
    }

    for (float y = fmodf(pan_offset.y, GRID_SIZE); y < canvas_size.y; y += GRID_SIZE)
    {
        draw_list->AddLine(ImVec2(origin.x, origin.y + y), ImVec2(origin.x + canvas_size.x, origin.y + y), IM_COL32(60, 60, 60, 255));
    }

    if (nodes.size() != lastUpdatedNodesSize)
    {
        ImVec2 mouse = ImGui::GetMousePos();
        ImVec2 local_pos = ImVec2(mouse.x - origin.x - pan_offset.x, mouse.y - origin.y - pan_offset.y);
        node_positions.push_back(local_pos);
        lastUpdatedNodesSize = nodes.size();
    }

    if (nodes.size() != 0)
    {
        for (int i = 0; i < nodes.size(); i++)
        {
            nodes[i].SpawnNode(draw_list, node_positions[i], pan_offset);
        }
    }
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
    {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        pan_offset.x += delta.x;
        pan_offset.y += delta.y;
    }

    ImGui::EndChild();
}

void GraphPanel::setPackages(const std::vector<std::string> &pkgs)
{
    ImGui::Text("Setting packages in GraphPanel");
    packages = pkgs;
}

void GraphPanel::GraphContextMenu()
{
    static std::string s_pkg;
    bool spawn_node = false;

    if (!ImGui::BeginPopup("GraphContextMenu"))
        return;

    if (ImGui::BeginMenu("Packages"))
    {
        if (packages.empty())
        {
            ImGui::TextDisabled("No packages loaded.");
        }
        else
        {
            for (const auto &pkg : packages)
            {
                if (ImGui::MenuItem(pkg.c_str()))
                {
                    s_pkg = pkg;
                    spawn_node = true;
                }
            }
        }
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Reset View"))
    {
        pan_offset = ImVec2(0.0f, 0.0f);
        zoom_level = 1.0f;
    }

    ImGui::EndPopup();
    if (spawn_node && !s_pkg.empty())
    {
        Node node(s_pkg, ImVec2(100.0f, 50.0f));
        nodes.push_back(node);
    }
}