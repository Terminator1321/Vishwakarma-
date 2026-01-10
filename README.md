# Vishwakarma-
Vishwakarma is an experimental **C++ editor-style application** that embeds a
**Python runtime** to build and visualize workflows using a dockable UI.
The project is focused on learning and exploring **systems-level integration**
between C++, Python, and modern UI tooling.

This is **not a finished product** — it is an evolving prototype (V1)

---

## Features (V1)

- Dockable editor UI using **ImGui + GLFW**
- Graph canvas with panning and context menus
- Embedded Python runtime using **pybind11**
- Python package discovery and caching
- Integrated terminal with basic commands and `pip` support
- JSON-based caching to avoid repeated Python calls
- Explicit runtime ownership and safe GIL handling

---

## Architecture Overview

- **C++** handles UI, editor logic, threading, and orchestration
- **Python** is embedded for package management and future execution logic
- A dedicated `PythonRuntime` owns the Python interpreter
- Panels are managed through a centralized `PanelManager`
- Data exchange is done via JSON files (temporary V1 design)

> ⚠️ Some parts are intentionally synchronous for correctness.
> Async behavior will be expanded in later versions.

---
## Requirements

### System
- Windows (tested on Windows 10/11)
- C++17 compatible compiler (MinGW / GCC)
- OpenGL-capable GPU

### Python
- Python **3.13**
- Virtual environment recommended

### Python packages
Installed via `requirements.txt`:
- `pybind11`

---
## Setup (Windows)

### 1. Clone the repository
```bash
git clone https://github.com/Terminator1321/Vishwakarma.git
cd Vishwakarma
```

The repository includes a command.txt file containing the required build commands to compile the source code into an executable (.exe) using MinGW/GCC.
Make sure your compiler, OpenGL libraries, and Python environment are correctly configured before building.
To get your pybind11 path
```bash
python -m pybind --Include
```
