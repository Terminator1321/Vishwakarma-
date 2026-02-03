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
    ImVec2 out_l(p1.x + width + 1.0f, p1.y + height * 0.5f);
    ImVec2 p2(p1.x + width, p1.y + height);
    ImGui::SetCursorScreenPos(p1);
    ImGui::InvisibleButton(node_name.c_str(), ImVec2(width, height));
    out_link = new link(out_l);
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
    InputsAOutputsPinHolders(draw_list, out_l, p1, theme_color, isSPECIALNODE, out_link);
}

void Node::InputsAOutputsPinHolders(ImDrawList *draw_list, ImVec2 out, ImVec2 in, color theme_color, bool isSpecial, link *outlink)
{
    ImGui::SetCursorScreenPos(out);
    ImGui::InvisibleButton("##", ImVec2(6.0f, 6.0f));
    draw_list->AddCircleFilled(out, 6.0f, theme_color.white());
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        out_link->DrawLink(draw_list,ImGui::GetMousePos());
    }
}

#pragma endregion

link::link(ImVec2 pos) : position(pos) {}
void link::DrawLink(ImDrawList *draw_list, ImVec2 p1)
{
    float dx = fabs(p1.x - position.x);
    float strength = dx * 0.5f;
    ImVec2 cp1(position.x + strength, position.y);
    ImVec2 cp2(p1.x - strength, p1.y);
    draw_list->AddBezierCubic(position, cp1, cp2, p1, theme_color.white(), 5.0f);
}

bool link::IsPinHovered(ImVec2 pin_pos, float radius)
{
    return false;
}
