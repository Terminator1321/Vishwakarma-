#ifndef PYTHONRUNTIME_HPP
#define PYTHONRUNTIME_HPP

#include <pybind11/embed.h>
#include <memory>

namespace py = pybind11;

class PythonRuntime {
public:
    PythonRuntime();

    py::gil_scoped_acquire acquireGIL();

private:
    std::unique_ptr<py::scoped_interpreter> interpreter;
};

#endif
