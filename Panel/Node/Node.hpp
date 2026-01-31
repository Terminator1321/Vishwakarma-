#ifndef NODE_HPP
#define NODE_HPP
#include"imgui.h"
#include"../GUIThemeHelper/theme.hpp"
#include <string>
#include <cstdint>
class Node {
    private:
        std::string node_name = "default node";
        ImVec2 size = ImVec2(100.0f, 50.0f);
        float padding = 10.0f;
        bool selected = false;
        bool active = false;
        color theme_color;
        bool isSPECIALNODE = false;
        int32_t id;
    public:
        Node(std::string node, ImVec2 size);
        void SpawnNode(ImDrawList* draw_list, ImVec2 canvas_origin, ImVec2 local_pos, ImVec2 pan_offset);
        // void calculateSize();
        bool isSelected() const { return selected; }
        void setSelected(bool sel) { selected = sel; }
        std::string getName() const { return node_name; }
        bool isActive() const { return active; }
        void setSPECIALNODE(bool special) { isSPECIALNODE = special; }
        bool isSPECIAL() const { return isSPECIALNODE; }
};

#endif