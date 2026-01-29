#ifndef COLOR_HPP
#define COLOR_HPP
#include "imgui.h"
class color
{
    public:
        ImU32 white();
        ImU32 black();
        ImU32 red();
        ImU32 green();
        ImU32 blue();
        ImU32 yellow();
        ImVec4 t_white();
        ImVec4 t_black();
        ImVec4 t_red();
        ImVec4 t_green();
        ImVec4 t_blue();
        ImVec4 t_yellow();
};
#endif