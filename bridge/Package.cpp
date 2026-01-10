#include "Package.hpp"
namespace py = pybind11;
using json = nlohmann::json;
void Package::save_installed_packages()
{
    py::module pkg = py::module::import("PY.save_package");
    pkg.attr("save_list_installed_packages")();
}

Package::Package(PythonRuntime &pythonruntime) : pythonruntime(pythonruntime) {}

void Package::refresh_async()
{
    ready = false;
    py::gil_scoped_acquire acquire;
    save_installed_packages();
    std::ifstream file("PY/PY_Data/installed_packages.json");
    if (!file.is_open())
        return;
    json j;
    file >> j;

    std::vector<std::string> temp;
    if (j.contains("packages") && j["packages"].is_array())
    {
        for (auto &p : j["packages"])
            temp.push_back(p.get<std::string>());
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        cached_packages = std::move(temp);
        ready = true;
    }
}

bool Package::is_ready() const
{
    return ready.load();
}

std::vector<std::string> Package::consume_packages()
{
    std::lock_guard<std::mutex> lock(mtx);
    ready = false;
    return cached_packages;
}
