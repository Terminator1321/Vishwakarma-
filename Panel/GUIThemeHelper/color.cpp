#include "color.hpp"

ImU32 color::white()
{
    return IM_COL32(255,255,255,255);
}

ImU32 color::black()
{
    return IM_COL32(0,0,0,255);
}

ImU32 color::green()
{
    return IM_COL32(0,255,0,255);
}

ImU32 color::red()
{
    return IM_COL32(255,0,0,255);
}

ImU32 color::blue()
{
    return IM_COL32(0,0,255,255);
}

ImU32 color::yellow()
{
    return IM_COL32(255,255,0,255);
}

ImVec4 color::t_white()
{
    return ImVec4(1.0f,1.0f,1.0f,1.0f);
}

ImVec4 color::t_black()
{
    return ImVec4(0,0,0,1.0f);
}

ImVec4 color::t_green()
{
    return ImVec4(0,1.0f,0,1.0f);
}

ImVec4 color::t_red()
{
    return ImVec4(1.0f,0,0,1.0f);
}

ImVec4 color::t_blue()
{
    return ImVec4(0,0,1.0f,1.0f);
}

ImVec4 color::t_yellow()
{
    return ImVec4(1.0f,1.0f,0,1.0f);
}