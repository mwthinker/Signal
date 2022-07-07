# mw::Signal ![CI build](https://github.com/mwthinker/Signal/actions/workflows/ci.yml/badge.svg)

A header only Signal Slot library inspired by the corresponding boost library.

The library is using C++20. While using Visual Studio, it must be at least version 12 or higher.

## Usage
Just copy the header file and license file.

## Building project localy
CMake and vcpkg is needed to run locally.

Inside the project folder, e.g.
```bash
cmake --preset=unix -B build -DSignal_Test=1 -DSignal_Example=1; cmake --build build; ctest --test-dir build/Signal_Test
./build/Signal_Example/Signal_Example
```

## Open source
The project is under the MIT license (see LICENSE.txt).
