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
    bool connected = false;
public:
    link(ImVec2 pos);
    void DrawLink(ImDrawList *draw_list, ImVec2 p1);
    bool IsPinHovered(ImVec2 pin_pos, float radius);
    bool isConnected() const { return connected; }
    void setConnected(bool conn) { connected = conn; }
    void UpdatePosition(ImVec2 pos) { position = pos; }
};

class Pins
{
    private:
        color theme_color;
        ImVec2 position;
        ImDrawList *current_draw_list;
        link *_link;
    public:
        Pins(ImVec2 pos, ImDrawList *draw_list);
        void UpdatePosition(ImVec2 pos) { position = pos; }
        void DrawPin(ImDrawList *draw_list, ImVec2 p1,ImVec2 p2);
        bool IsPinHovered(ImVec2 pin_pos, float radius);
        void OnMouseDragBeginOverEvent();
};

class Node
{
    private:
        struct NodeData{
            // std::vector<ImU32> colors; for future use
            // std::vector<std::string> datatypes; for future use
            std::vector<Pins> pins;
            std::vector<ImVec2> pin_positions;
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
        ImDrawList *current_draw_list;
    public:
        Node(std::string node, ImVec2 size);
        void SpawnNode(ImDrawList *draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset);
        bool isSelected() const { return selected; }
        void setSelected(bool sel) { selected = sel; }
        std::string getName() const { return node_name; }
        bool isActive() const { return active; }
        void setSPECIALNODE(bool special) { isSPECIALNODE = special; }
        bool isSPECIAL() const { return isSPECIALNODE; }
        
};

#endif