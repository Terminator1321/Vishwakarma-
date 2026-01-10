#include "PythonRuntime.hpp"

PythonRuntime::PythonRuntime()
{
    interpreter = std::make_unique<py::scoped_interpreter>();
}

py::gil_scoped_acquire PythonRuntime::acquireGIL()
{
    return py::gil_scoped_acquire();
}
