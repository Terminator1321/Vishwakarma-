@echo off
setlocal EnableExtensions EnableDelayedExpansion

set PYTHON=C:/Users/Rohan/AppData/Local/Programs/Python/Python313
set GLFW=glfw
set EXE=vishwakarma.exe

echo ===============================
echo Building Vishwakarma...
echo ===============================

g++ main.cpp ^
-I%PYTHON%/Include ^
-I%PYTHON%/Lib/site-packages/pybind11/include ^
-Ibridge ^
-Iimgui ^
-Iimgui/backends ^
-I%GLFW%/include ^
imgui/imgui.cpp ^
imgui/imgui_draw.cpp ^
imgui/imgui_tables.cpp ^
imgui/imgui_widgets.cpp ^
imgui/imgui_demo.cpp ^
imgui/backends/imgui_impl_glfw.cpp ^
imgui/backends/imgui_impl_opengl3.cpp ^
bridge/runtime/PythonRuntime.cpp ^
bridge/Package.cpp ^
Panel/GUIThemeHelper/theme.cpp ^
Panel/helper_functions/Windows.cpp ^
Panel/Node/Node.cpp ^
Panel/Panel.cpp ^
-L%PYTHON%/libs ^
-L%GLFW%/lib ^
-lpython313 ^
-lglfw3 ^
-lopengl32 ^
-lgdi32 ^
-std=gnu++17 ^
-o %EXE%

if errorlevel 1 (
    echo.
    echo BUILD FAILED
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESS
echo Output: %EXE%
echo.

choice /m "Do you want to run Vishwakarma now?"

if errorlevel 2 (
    echo Not running.
    exit /b 0
)

echo.
echo Launching Vishwakarma...
echo.

%EXE%
