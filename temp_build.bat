@echo off
cd /d "%~dp0"
set VCPKG_ROOT=
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cd backend
mkdir build
cd build
cmake -G "Visual Studio 18 2026" -A x64 .. "-DCMAKE_TOOLCHAIN_FILE=..\..\vcpkg\scripts\buildsystems\vcpkg.cmake"