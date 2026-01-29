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
};
#endif