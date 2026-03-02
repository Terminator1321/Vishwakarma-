#include "Node.hpp"

#pragma region Node Implementation

Node::Node(std::string node, ImVec2 size) : node_name(node), size(size)
{
    for (int i = 0; i < inputpins; ++i)
    {
        ImVec2 pin_pos(0.0f, (size.y / (inputpins + 1)) * (i + 1));
        input_data.pins.emplace_back(Pins(pin_pos, 7.0f, true));
        input_data.pin_names.push_back("In " + std::to_string(i + 1));
    }
    for (int i = 0; i < outputpins; ++i)
    {
        ImVec2 pin_pos(size.x, (size.y / (outputpins + 1)) * (i + 1));
        output_data.pins.emplace_back(Pins(pin_pos, 7.0f, false));
        output_data.pin_names.push_back("Out " + std::to_string(i + 1));
    }
}

void Node::RegisterPins(std::vector<Pins *> &all_input_pins, std::vector<Pins *> &all_output_pins)
{
    for (auto &pin : input_data.pins)
        all_input_pins.push_back(&pin);
    for (auto &pin : output_data.pins)
        all_output_pins.push_back(&pin);
}

void Node::SpawnNode(ImDrawList *draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset,std::vector<Pins *> &all_input_pins, std::vector<Pins *> &all_output_pins)
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
        selected = true;
    if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        selected = false;

    draw_list->AddRectFilled(p1, p2, (isSPECIALNODE ? theme_color.red() : theme_color.blue()), 6.0f);
    if (selected)
        draw_list->AddRect(p1, p2, (isSPECIALNODE ? theme_color.green() : theme_color.yellow()), 6.0f, 0, 8.0f);

    draw_list->AddText(ImVec2(p1.x + padding, p1.y + padding), theme_color.white(), node_name.c_str());

    for (int i = 0; i < inputpins && !isSPECIALNODE; ++i)
    {
        ImVec2 pin_pos(p1.x, p1.y + (height / (inputpins + 1)) * (i + 1));
        input_data.pins[i].DrawPin(draw_list, pin_pos, ImVec2(0, 0));
        // ConnectTO must come BEFORE OnMouseDragBeginOverEvent so isBegienDrag
        // is still true on the release frame when ConnectTO needs to read it.
        input_data.pins[i].ConnectTOClosestSTypePin(draw_list, all_output_pins);
        input_data.pins[i].OnMouseDragBeginOverEvent(draw_list);
    }

    for (int i = 0; i < outputpins; ++i)
    {
        ImVec2 pin_pos(p1.x + width, p1.y + (height / (outputpins + 1)) * (i + 1));
        output_data.pins[i].DrawPin(draw_list, pin_pos, ImVec2(0, 0));
        // Same order fix: ConnectTO before OnMouseDragBeginOverEvent
        output_data.pins[i].ConnectTOClosestSTypePin(draw_list, all_input_pins);
        output_data.pins[i].OnMouseDragBeginOverEvent(draw_list);
    }
}

#pragma endregion

#pragma region Link Implementation

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

Pins::Pins(ImVec2 pos, float radius, bool isInput) : position(pos), radius(radius), isInput(isInput)
{
    _link = new link(position);
}

void Pins::DrawPin(ImDrawList *draw_list, ImVec2 p1, ImVec2 p2)
{
    UpdatePosition(p1);
    _link->UpdatePosition(p1);
    draw_list->AddCircleFilled(position, radius, theme_color.white());
}

bool Pins::IsPinHovered(ImVec2 pin_pos, float radius)
{
    ImVec2 mos_pos = ImGui::GetIO().MousePos;
    float dx = mos_pos.x - pin_pos.x;
    float dy = mos_pos.y - pin_pos.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void Pins::OnMouseDragBeginOverEvent(ImDrawList *draw_list)
{
    // Redraw persistent connection every frame
    if (_link && _link->isConnected() && connected_pin)
    {
        _link->UpdateEndPosition(connected_pin->GetPosition());
        _link->DrawLink(draw_list, _link->getEndPosition());
    }

    // Start drag when clicking on a pin
    if (!isBegienDrag && IsPinHovered(position, radius) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        isBegienDrag = true;
    }

    // Draw in-progress drag line — also draw on release frame so there's no gap
    // before ConnectTO fires. isBegienDrag is only reset inside ConnectTO now.
    if (isBegienDrag && (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left)))
    {
        _link->DrawLink(draw_list, ImGui::GetIO().MousePos);
    }

}

void Pins::ConnectTOClosestSTypePin(ImDrawList *draw_list, std::vector<Pins *> &candidate_pins)
{
    if (!isBegienDrag)
        return;
    if (!ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        return;

    ImVec2 mousePos = ImGui::GetIO().MousePos;

    for (int i = 0; i < (int)candidate_pins.size(); i++)
    {
        Pins *candidate = candidate_pins[i];

        if (candidate == this)
            continue;
        if (candidate->GetConnectionPin() != nullptr)
            continue;

        ImVec2 pinPos = candidate->GetPosition();
        float dx = mousePos.x - pinPos.x;
        float dy = mousePos.y - pinPos.y;
        float distSq = dx * dx + dy * dy;

        printf("Checking candidate pin %d at screen (%.1f, %.1f) vs mouse (%.1f, %.1f) distSq=%.1f threshold=%.1f\n",
               i, pinPos.x, pinPos.y, mousePos.x, mousePos.y, distSq, 30.0f * 30.0f);

        if (distSq <= (30.0f * 30.0f))
        {
            printf(">> Connected to pin %d!\n", i);
            _link->UpdateEndPosition(pinPos);
            SetConnectionPin(candidate);
            candidate->SetConnectionPin(this);
            _link->setConnected(true);
            isBegienDrag = false;
            return;
        }
    }

    printf("No pin in range, drag cancelled.\n");
    isBegienDrag = false; 
}

#pragma endregion