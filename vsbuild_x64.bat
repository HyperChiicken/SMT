@echo off
mkdir build64 > nul 2> nul
cd build64
set SCRIPT_DIR=%~dp0
set CMAKE_PREFIX_PATH=C:\Qt\5.12.3\msvc2017_64
cmake -G"Visual Studio 17 2022" -A "x64" -DWINDOWS_SERVICE=OFF -DCMAKE_INSTALL_PREFIX="%SCRIPT_DIR%\build64\install" -DCPACK_WIX_ROOT="C:/Program Files (x86)/WiX Toolset v3.11" -DUSE_SYSTEM_OPENSSL=ON ..
pause
