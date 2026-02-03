#include "Node.hpp"

#pragma region Node Implementation

// Node implementation
Node::Node(std::string node, ImVec2 size) : node_name(node), size(size)
{
    for (int i = 0; i < inputpins; ++i)
    {
        ImVec2 pin_pos(0.0f, (size.y / (inputpins + 1)) * (i + 1));
        input_data.pins.emplace_back(Pins(pin_pos, current_draw_list));
        input_data.pin_positions.push_back(pin_pos);
        input_data.pin_names.push_back("In " + std::to_string(i + 1)); // we will change it in future with the real name of input parameters
    }
    for (int i = 0; i < outputpins; ++i)
    {
        ImVec2 pin_pos(size.x, (size.y / (outputpins + 1)) * (i + 1));
        output_data.pins.emplace_back(Pins(pin_pos, current_draw_list));
        output_data.pin_positions.push_back(pin_pos);
        output_data.pin_names.push_back("Out " + std::to_string(i + 1)); // we will change it in future with the real name of return types
    }
}

void Node::SpawnNode(ImDrawList *draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset)
{
    current_draw_list = draw_list;
    ImVec2 p1(canvas_origin.x + local_pos.x + pan_offset.x, canvas_origin.y + local_pos.y + pan_offset.y);
    ImVec2 text_size = ImGui::CalcTextSize(node_name.c_str());

    float width = std::max(size.x, text_size.x + padding * 2.0f);
    float height = std::max(size.y, text_size.y + padding * 2.0f);

    ImVec2 out_l(p1.x + width + 1.0f, p1.y + height * 0.5f);
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
    current_draw_list->AddRectFilled(p1, p2, (isSPECIALNODE ? theme_color.red() : theme_color.blue()), 6.0f);
    if (selected)
    {
        current_draw_list->AddRect(p1, p2, (isSPECIALNODE ? theme_color.green() : theme_color.yellow()), 6.0f, 0, 8.0f);
    }

    current_draw_list->AddText(ImVec2(p1.x + padding, p1.y + padding), theme_color.white(), node_name.c_str());
    for (int i = 0; i < inputpins; ++i)
    {
        ImVec2 pin_pos(p1.x, p1.y + (height / (inputpins + 1)) * (i + 1));
        input_data.pins[i].DrawPin(current_draw_list, pin_pos, ImVec2(0, 0));
        input_data.pin_positions[i] = pin_pos;
    }
    for (int i = 0; i < outputpins; ++i)
    {
        ImVec2 pin_pos(p1.x + width, p1.y + (height / (outputpins + 1)) * (i + 1));
        output_data.pins[i].DrawPin(current_draw_list, pin_pos, ImVec2(0, 0));
        output_data.pin_positions[i] = pin_pos;
    }
}

#pragma endregion

#pragma region Link Implementation
// Link implementation
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

#pragma endregion

#pragma region Pins Implementation
// Pins implementation
Pins::Pins(ImVec2 pos, ImDrawList *draw_list) : position(pos), current_draw_list(draw_list)
{
    _link = new link(position);
};

void Pins::DrawPin(ImDrawList *draw_list, ImVec2 p1, ImVec2 p2)
{
    UpdatePosition(p1);
    _link->UpdatePosition(position);
    draw_list->AddCircleFilled(position, 5.0f, theme_color.white()); // hard coded radius for now
}

bool Pins::IsPinHovered(ImVec2 pin_pos, float radius)
{
    ImVec2 mos_pos = ImGui::GetIO().MousePos;
    float dx = mos_pos.x - pin_pos.x;
    float dy = mos_pos.y - pin_pos.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void Pins::OnMouseDragBeginOverEvent()
{
    if (!IsPinHovered(position, 5.0f))
        return; // hard coded radius for now
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        _link->DrawLink(current_draw_list, ImGui::GetIO().MousePos);
    }
    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !_link->isConnected())
    {
        // Here we will check if we are releasing over another pin to create a connection
    }
}

#pragma endregion