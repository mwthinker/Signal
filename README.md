# mw::Signal [![CI build](https://github.com/mwthinker/Signal/actions/workflows/ci.yml/badge.svg)](https://github.com/mwthinker/Signal/actions/workflows/ci.yml)

A header only Signal Slot library inspired by the corresponding boost library.

The library is using C++20. While using Visual Studio, it must be at least version 12 or higher. Can also be build by using Clang version >=14 or GCC version >= 11.

## Usage
Just copy the header file and license file. No external dependencies are needed.

### Using CMake
When using [CMake](https://cmake.org/) it could be included by:

```cmake
FetchContent_Declare(Signal
    GIT_REPOSITORY
        https://github.com/mwthinker/Signal.git
    GIT_TAG
        1f7cf5097a1fdfdefe49d78e215cf12653407629 # commit SHA of Signal to use
    OVERRIDE_FIND_PACKAGE # CMake version must be >= 3.24 to support this
)

target_link_libraries(YourProject
    PRIVATE
        Signal
)
```

## Code example
```cpp
#include <mw/signal.h>
#include <iostream>

int main() {
    mw::Signal<int> signal;
    
    auto connection = signal.connect([](int someInteger) {
        std::cout << "Hello world! Number " << someInteger << "\n"; 
    });

    signal(10);
    signal(15);

    return 0;
}
```
Result:
```bash
Hello world! Number 10
Hello world! Number 15
```
For more example code see [Signal_Example](https://github.com/mwthinker/Signal/blob/master/Signal_Example/src/main.cpp) or [Signal_Test](https://github.com/mwthinker/Signal/blob/master/Signal_Test/src/signaltests.cpp).

## Building project locally
To build localy [vcpkg](https://github.com/microsoft/vcpkg.git) can be used to insert GTest dependency to test project.

Inside this repository folder, e.g. run:
```bash
cmake --preset=unix -B build -DSignal_Test=1 -DSignal_Example=1; cmake --build build; ctest --test-dir build/Signal_Test
./build/Signal_Example/Signal_Example
```

## Open source
The project is under the MIT license (see LICENSE.txt).
