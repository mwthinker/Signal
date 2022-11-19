# mw::Signal [![CI build](https://github.com/mwthinker/Signal/actions/workflows/ci.yml/badge.svg)](https://github.com/mwthinker/Signal/actions/workflows/ci.yml)

A header only Signal Slot library inspired by the corresponding boost library.

The library is using C++20. While using Visual Studio, it must be at least version 12 or higher.

## Usage
Just copy the header file and license file. No external dependencies are needed.

### CMake
Most simple way to use it to include in your CMake file:

```cmake
FetchContent_Declare(Signal
	GIT_REPOSITORY
		https://github.com/mwthinker/Signal.git
	GIT_TAG
		1f7cf5097a1fdfdefe49d78e215cf12653407629 # commit version of Signal to use
	OVERRIDE_FIND_PACKAGE # CMake version must be >= 3.24 to support this
)
```

And then use it by replaceing "YourProject"
```cmake
target_link_libraries(YourProject
	PRIVATE
		Signal
)
```

Example code in your C++ code:
```cpp
#include <mw/signal.h>
#include <iostream>

int main() {
    mw::Signal<int> signal;
    
    auto connection = signal.connect([](int someInteger) {
		std::cout << "Hello world! Number " << someInteger << "\n"; 
	});

    signal(10);

    return 0;
}

```
> Hello world! Number 10

## Building project locally
CMake and vcpkg is needed to run locally.

Inside the project folder, e.g.
```bash
cmake --preset=unix -B build -DSignal_Test=1 -DSignal_Example=1; cmake --build build; ctest --test-dir build/Signal_Test
./build/Signal_Example/Signal_Example
```

## Open source
The project is under the MIT license (see LICENSE.txt).
