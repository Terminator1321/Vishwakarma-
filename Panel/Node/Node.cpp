#include "Node.hpp"

#pragma region Node Implementation

// Node implementation
Node::Node(std::string node, ImVec2 size) : node_name(node), size(size)
{
    for (int i = 0; i < inputpins; ++i)
    {
        ImVec2 pin_pos(0.0f, (size.y / (inputpins + 1)) * (i + 1));
        input_data.pins.emplace_back(Pins(pin_pos, 7.0f));
        input_data.pin_positions.push_back(pin_pos);
        input_data.pin_names.push_back("In " + std::to_string(i + 1)); // we will change it in future with the real name of input parameters
    }
    for (int i = 0; i < outputpins; ++i)
    {
        ImVec2 pin_pos(size.x, (size.y / (outputpins + 1)) * (i + 1));
        output_data.pins.emplace_back(Pins(pin_pos, 7.0f));
        output_data.pin_positions.push_back(pin_pos);
        output_data.pin_names.push_back("Out " + std::to_string(i + 1)); // we will change it in future with the real name of return types
    }
}

// helper function
ImVec2 GetMouse2NodePin(float x,float y)
{
    ImVec2 mouse_screen = ImGui::GetMousePos();
    ImVec2 mouse_node_local = ImVec2(mouse_screen.x - x , mouse_screen.y - y);
    return mouse_node_local;
}

void Node::SpawnNode(ImDrawList *draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset)
{
    ImVec2 p1(canvas_origin.x + local_pos.x + pan_offset.x, canvas_origin.y + local_pos.y + pan_offset.y);
    float x= p1.x;
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
    draw_list->AddRectFilled(p1, p2, (isSPECIALNODE ? theme_color.red() : theme_color.blue()), 6.0f);
    if (selected)
    {
        draw_list->AddRect(p1, p2, (isSPECIALNODE ? theme_color.green() : theme_color.yellow()), 6.0f, 0, 8.0f);
    }

    draw_list->AddText(ImVec2(p1.x + padding, p1.y + padding), theme_color.white(), node_name.c_str());
    for (int i = 0; i < inputpins && !isSPECIALNODE; ++i)
    {
        ImVec2 pin_pos(p1.x, p1.y + (height / (inputpins + 1)) * (i + 1));
        input_data.pins[i].DrawPin(draw_list, pin_pos, ImVec2(0, 0));
        input_data.pin_positions[i] = pin_pos;
        input_data.pins[i].OnMouseDragBeginOverEvent(draw_list);
        input_data.pins[i].ConnectTOClosestSTypePin(draw_list, output_data.pin_positions, output_data.pins, GetMouse2NodePin(x,p1.y));
    }
    for (int i = 0; i < outputpins; ++i)
    {
        ImVec2 pin_pos(p1.x + width, p1.y + (height / (outputpins + 1)) * (i + 1));
        output_data.pins[i].DrawPin(draw_list, pin_pos, ImVec2(0, 0));
        output_data.pin_positions[i] = pin_pos;
        output_data.pins[i].OnMouseDragBeginOverEvent(draw_list);
        output_data.pins[i].ConnectTOClosestSTypePin(draw_list, input_data.pin_positions, input_data.pins, GetMouse2NodePin(x,p1.y));
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
Pins::Pins(ImVec2 pos, float radius) : position(pos), radius(radius)
{
    _link = new link(position);
}

void Pins::DrawPin(ImDrawList *draw_list, ImVec2 p1, ImVec2 p2)
{
    UpdatePosition(p1);
    _link->UpdatePosition(position);
    draw_list->AddCircleFilled(position, radius, theme_color.white());
}

bool Pins::IsPinHovered(ImVec2 pin_pos, float radius)
{
    ImVec2 mos_pos = ImGui::GetIO().MousePos;
    float dx = mos_pos.x - pin_pos.x;
    float dy = mos_pos.y - pin_pos.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// helper function to handle joints in links`
bool IsInRange(float max_distance, ImVec2 p1, ImVec2 p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance <= max_distance;
}

void Pins::OnMouseDragBeginOverEvent(ImDrawList *draw_list)
{

    if (_link && _link->isConnected())
    {
        printf("Already Connected\n");
        _link->UpdateEndPosition(GetConnectionPin()->position);
        _link->DrawLink(draw_list, _link->getEndPosition());
    }

    if (!isBegienDrag && IsPinHovered(position, radius) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        isBegienDrag = true;
    }

    if (isBegienDrag && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && _link)
    {
        _link->DrawLink(draw_list, ImGui::GetIO().MousePos);
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && _link->isConnected())
    {
        isBegienDrag = false;
    }
}

bool _IsInRange(float max_distance, ImVec2 p1, ImVec2 p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return (dx * dx + dy * dy) <= (max_distance * max_distance);
}

void Pins::ConnectTOClosestSTypePin(ImDrawList *draw_list, std::vector<ImVec2> &pos, std::vector<Pins> &pins, ImVec2 mousePosInWindow)
{
    if (!isBegienDrag)
        return;

    if (!ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        return;

    for (int i = 0; i < pos.size(); i++)
    {
        // for debug purposes
        draw_list->AddCircleFilled(pos[i], 30.0f, theme_color.red());
        draw_list->AddCircleFilled(mousePosInWindow, 30.0f, theme_color.green());
        draw_list->AddCircleFilled(ImGui::GetIO().MousePos, 30.0f, theme_color.blue());

        printf("Checking pin %d\n", i);
        ImVec2 p = pos[i];
        printf("Pin position: (%f, %f)\n", p.x, p.y);
        printf("Mouse position in window: (%f, %f)\n", mousePosInWindow.x, mousePosInWindow.y);
        if (_IsInRange(30.0f, mousePosInWindow, p))
        {
            printf("Pin %d is in range\n", i);
            _link->UpdateEndPosition(p);
            printf("Connecting to pin %d\n", i);
            SetConnectionPin(&pins[i]);
            printf("Setting connection pin\n");
            pins[i].SetConnectionPin(this);
            _link->setConnected(true);
            isBegienDrag = false;
            printf("Connected!\n");
            _link->DrawLink(draw_list, _link->getEndPosition());
            return;
        }
    }
    isBegienDrag = false;
}

#pragma endregion