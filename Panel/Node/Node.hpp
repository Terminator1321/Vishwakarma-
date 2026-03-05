#ifndef NODE_HPP
#define NODE_HPP
#include "imgui.h"
#include "../GUIThemeHelper/theme.hpp"
#include <string>
#include <cstdint>
#include <cmath>
#include <vector>

class link
{
private:
    color theme_color;
    ImVec2 position;
    ImVec2 end_position;
    bool connected = false;

public:
    link(ImVec2 pos);
    void DrawLink(ImDrawList *draw_list, ImVec2 p1);
    bool IsPinHovered(ImVec2 pin_pos, float radius);
    bool isConnected() const { return connected; }
    void setConnected(bool conn) { connected = conn; }
    void UpdatePosition(ImVec2 pos) { position = pos; }
    void UpdateEndPosition(ImVec2 pos) { end_position = pos; }
    ImVec2 getEndPosition() const { return end_position; }
};
class Node;
class Pins
{
private:
    color theme_color;
    ImVec2 position;
    link *_link;
    bool isBegienDrag = false;
    float radius = 5.0f;
    Node *owner_node = nullptr;
    Pins *connected_pin = nullptr;
    bool isInput = false;

public:
    Pins(ImVec2 pos, float radius, bool isInput = false);
    void UpdatePosition(ImVec2 pos) { position = pos; }
    ImVec2 GetPosition() const { return position; }
    bool IsInputPin() const { return isInput; }
    void DrawPin(ImDrawList *draw_list, ImVec2 p1, ImVec2 p2);
    bool IsPinHovered(ImVec2 pin_pos, float radius);
    void OnMouseDragBeginOverEvent(ImDrawList *draw_list);

    // candidate_pins: ALL pins of the OPPOSITE type from ALL nodes in the scene
    void ConnectTOClosestSTypePin(ImDrawList *draw_list, std::vector<Pins *> &candidate_pins);
    Node *GetOwner() { return owner_node; }
    void SetOwner(Node *n) { owner_node = n; }
    void SetConnectionPin(Pins *pin) { connected_pin = pin; }
    Pins *GetConnectionPin() const { return connected_pin; }
    bool IsDragging() const { return isBegienDrag; }
};

class Node
{
private:
    struct NodeData
    {
        std::vector<Pins> pins;
        std::vector<std::string> pin_names;
    };
    std::string node_name = "default node";
    ImVec2 size = ImVec2(100.0f, 50.0f);
    float padding = 10.0f;
    bool selected = false;
    bool active = false;
    color theme_color;
    bool isSPECIALNODE = false;
    int32_t id;
    int inputpins = 1;
    int outputpins = 1;
    NodeData input_data;
    NodeData output_data;

public:
    Node(std::string node, ImVec2 size);
    void RegisterPins(std::vector<Pins *> &all_input_pins, std::vector<Pins *> &all_output_pins);
    void SpawnNode(ImDrawList *draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset, std::vector<Pins *> &all_input_pins, std::vector<Pins *> &all_output_pins);

    bool isSelected() const { return selected; }
    void setSelected(bool sel) { selected = sel; }
    std::string getName() const { return node_name; }
    bool isActive() const { return active; }
    void setSPECIALNODE(bool special) { isSPECIALNODE = special; }
    bool isSPECIAL() const { return isSPECIALNODE; }
    Pins *GetOutputPin(int index)
    {
        if (index < 0 || index >= (int)output_data.pins.size())
            return nullptr;
        return &output_data.pins[index];
    }
    Pins *GetInputPin(int index)
    {
        if (index < 0 || index >= (int)input_data.pins.size())
            return nullptr;
        return &input_data.pins[index];
    }
};

#endif