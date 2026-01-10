#ifndef WINDOWS_HPP
#define WINDOWS_HPP
#include<iostream>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
class Windows
{
    private:
        int width;
        int height;
        const std::string title;
    public:
        Windows(int width, int height, const std::string title);
        virtual void addComponent() = 0;
        void run();
};
#endif