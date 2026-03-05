#include "PythonRuntime.hpp"

PythonRuntime::PythonRuntime()
{
    interpreter = std::make_unique<py::scoped_interpreter>();
    gil_release = std::make_unique<py::gil_scoped_release>();
}

py::gil_scoped_acquire PythonRuntime::acquireGIL()
{
    return py::gil_scoped_acquire();
}
