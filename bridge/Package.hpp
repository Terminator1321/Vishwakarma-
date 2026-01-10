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
#include <inc/json.hpp>
#include "runtime/PythonRuntime.hpp"

class Package
{
public:
    Package(PythonRuntime &pythonruntime);
    void refresh_async();
    bool is_ready() const;
    std::vector<std::string> consume_packages();
    
private:
    void save_installed_packages();
    std::vector<std::string> cached_packages;
    std::atomic<bool> ready{false};
    std::mutex mtx;
    PythonRuntime &pythonruntime;
};

#endif