#ifndef PACKAGE_HPP
#define PACKAGE_HPP

#include <pybind11/embed.h>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <queue>
#include <inc/json.hpp>
#include "runtime/PythonRuntime.hpp"

class Package
{
    public:
        Package(PythonRuntime &pythonruntime);
        void refresh_async();
        bool is_ready() const;
        std::vector<std::string> consume_packages();
        std::unordered_map<std::string, py::module_> module_cache;
        std::queue<std::string> errors;
        std::queue<std::string> warnings;
        std::queue<std::string> outputs;
    
    private:
        void save_installed_packages();
        std::vector<std::string> cached_packages;
        std::atomic<bool> ready{false};
        std::mutex mtx;
        PythonRuntime &pythonruntime;
        void import_module(const std::vector<std::string> &packages);
};

#endif