#ifndef PANEL_HPP
#define PANEL_HPP
#include "helper_functions/Windows.hpp"
#include "GUIThemeHelper/color.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <cstdio>
#include <array>
#include <thread>
#include <queue>

class Terminal : public Windows
{
    private:
        color terminal_color;

    protected:
        std::vector<std::string> command_history;
        char command[512] = "";
        void exec_cmd(const std::string &cmd);
        void run_pip_async(const std::string &cmd);

    public:
        Terminal(int width, int height, const std::string title);
        void addComponent() override;
        void AddErrorMessage(const std::string &msg);
        void AddOutputMessage(const std::string &msg);
        void AddWarningMessage(const std::string &msg);
};

class ImportPanel : public Windows
{
    protected:
        std::vector<std::string> packages;
        bool force_update = false;
    public:
        ImportPanel(int width, int height, const std::string title);
        void addComponent() override;
        void setPackages(const std::vector<std::string> &pkgs);
        std::vector<std::string> getPackages() const;
        bool getUpdate();
        void setUpdate(bool fu);
};

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
};
class GraphPanel : public Windows
{
    private:
        std::string PANELTYPE;  
        ImVec2 pan_offset = ImVec2(0.0f, 0.0f);
        float zoom_level = 1.0f;
        std::vector<std::string> packages;
        std::vector<Node> nodes;
        std::vector<ImVec2> node_positions;
        int lastUpdatedNodesSize = 0;
        bool isNodeRepeated(const std::string &node_name);
    public:
        std::queue<std::string> errors;
        std::queue<std::string> warnings;
        std::queue<std::string> outputs;
        GraphPanel(int width, int height, const std::string title, const std::string paneltype);
        void addComponent() override;
        void setPackages(const std::vector<std::string> &pkgs);
        void GraphContextMenu();
};

#endif