#include "Panel.hpp"
#pragma region Windows Implementation
// Base window runner to centralize ImGui Begin/End logic.
void Windows::run()
{
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once);
    ImGui::Begin(title.c_str());
    addComponent();
    ImGui::End();
}
#pragma endregion0

#pragma region Terminal Implementations
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
        ImGui::PushFont(font_helper.getMomoFont());

        if (line.rfind("[ERROR]", 0) == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, terminal_color.t_red());
        }
        else if (line.rfind("[WARNING]", 0) == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, terminal_color.t_yellow());
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, terminal_color.t_white());
        }

        ImGui::TextUnformatted(line.c_str());

        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    ImGui::Separator();

    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##terminal_input", Terminal::command, IM_ARRAYSIZE(Terminal::command), ImGuiInputTextFlags_EnterReturnsTrue))
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
            Terminal::command_history.push_back("Use Imports panel");
        }
        else if (cmd.rfind("pip", 0) == 0)
        {
            Terminal::command_history.push_back("Running pip command asynchronously...");
            Terminal::run_pip_async(cmd);
        }
        else
            Terminal::command_history.push_back("[ERROR] Unknown command: " + cmd);

        Terminal::command[0] = '\0';
        ImGui::SetKeyboardFocusHere(-1);
    }
    ImGui::PopItemWidth();
}

void Terminal::AddErrorMessage(const std::string &msg)
{
    command_history.push_back("[ERROR] " + msg);
}

void Terminal::AddOutputMessage(const std::string &msg)
{
    command_history.push_back(msg);
}

void Terminal::AddWarningMessage(const std::string &msg)
{
    command_history.push_back("[WARNING] " + msg);
}
#pragma endregion

#pragma region Import Implementations
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
#pragma endregion

#pragma region Graph Implementations
// Graph class implementation
GraphPanel::GraphPanel(int width, int height, const std::string title, const std::string paneltype) : Windows(width, height, title), PANELTYPE(paneltype)
{
    if (PANELTYPE == "Import")
    {
        Node import_node("Import Node", ImVec2(100.0f, 50.0f));
        import_node.setSPECIALNODE(true);
        nodes.push_back(import_node);
        node_positions.push_back(ImVec2(50.0f, 50.0f));
    }
    if (PANELTYPE == "Runtime")
    {
        // TODO: Add runtime specific nodes
    }
}

// helper function for node positioning
ImVec2 GetPositionWithMouseInput(ImVec2 pan_offset)
{
    ImVec2 mouse = ImGui::GetMousePos();
    ImVec2 _origin = ImGui::GetWindowPos();
    ImVec2 local_pos = ImVec2(mouse.x - _origin.x - pan_offset.x, mouse.y - _origin.y - pan_offset.y);
    return local_pos;
}

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
        ImVec2 local_pos = GetPositionWithMouseInput(pan_offset);
        node_positions.push_back(local_pos);
        lastUpdatedNodesSize = nodes.size();
    }

    if (nodes.size() != 0)
    {
        for (int i = 0; i < nodes.size(); i++)
        {
            if (nodes[i].isSelected() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && nodes[i].isActive())
            {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                node_positions[i].x += delta.x;
                node_positions[i].y += delta.y;
                nodes[i].SpawnNode(draw_list, origin, node_positions[i], pan_offset);
            }
            else
            {
                nodes[i].SpawnNode(draw_list, origin, node_positions[i], pan_offset);
            }
            if (nodes[i].isSelected() && ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                if (nodes[i].isSPECIAL())
                {
                    warnings.push("Cannot delete special node: " + nodes[i].getName());
                    break;
                }
                nodes.erase(nodes.begin() + i);
                node_positions.erase(node_positions.begin() + i);
                lastUpdatedNodesSize = nodes.size();
                break;
            }
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
    if (spawn_node && !s_pkg.empty() && !isNodeRepeated(s_pkg))
    {
        Node node(s_pkg, ImVec2(100.0f, 50.0f));
        nodes.push_back(node);
    }
}
bool GraphPanel::isNodeRepeated(const std::string &node_name)
{
    if (nodes.empty())
        return false;
    for (const auto &node : nodes)
    {
        if (node.getName() == node_name)
        {
            warnings.push("Node " + node_name + " already exists! cannot add duplicate nodes.");
            return true;
        }
    }
    return false;
}
#pragma endregion