#include "Node.hpp"

#pragma region Node Implementation
// Node implementation
Node::Node(std::string node, ImVec2 size) : node_name(node), size(size) {}

void Node::SpawnNode(ImDrawList *draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset)
{
    ImVec2 p1(canvas_origin.x + local_pos.x + pan_offset.x, canvas_origin.y + local_pos.y + pan_offset.y);
    ImVec2 text_size = ImGui::CalcTextSize(node_name.c_str());

    float width = std::max(size.x, text_size.x + padding * 2.0f);
    float height = std::max(size.y, text_size.y + padding * 2.0f);

    ImVec2 p2(p1.x + width, p1.y + height);
    ImGui::SetCursorScreenPos(p1);
    ImGui::InvisibleButton(node_name.c_str(), ImVec2(width, height));
    active = ImGui::IsItemActive();
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && active)
    {
        selected = true;
    }
    if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        selected = false;
    }
    draw_list->AddRectFilled(p1, p2, (isSPECIALNODE ? theme_color.red() : theme_color.blue()), 6.0f);
    if (selected)
    {
        draw_list->AddRect(p1, p2, (isSPECIALNODE ? theme_color.green() : theme_color.yellow()), 6.0f, 0, 8.0f);
    }

    draw_list->AddText(ImVec2(p1.x + padding, p1.y + padding), theme_color.white(), node_name.c_str());
}

#pragma endregion