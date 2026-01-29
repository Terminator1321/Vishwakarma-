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
    try
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
        outputs.push("Package list refreshed successfully.");
    }
    catch (const py::error_already_set &e)
    {
        std::lock_guard<std::mutex> lock(mtx);
        errors.push(e.what());
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

void Package::import_module(const std::vector<std::string> &packages)
{
    for (const auto &pkg : packages)
    {
        if (module_cache.find(pkg) != module_cache.end())
            continue;

        try
        {
            py::module mod = py::module::import(pkg.c_str());
            module_cache.emplace(pkg, mod);
            outputs.push("Successfully imported package: " + pkg);
        }
        catch (const py::error_already_set &e)
        {
            std::lock_guard<std::mutex> lock(mtx);
            errors.push("Failed to import package: " + pkg + "\nError: " + e.what());
        }
    }
}
