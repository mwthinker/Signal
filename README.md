Signal
======
Is a simple Signal/Slot library inspired by the corresponding boost library.

The library is using C++17. While using Visual Studio, it must be at least version 12 or higher.

<p><b>Open source</b></p>
The project is under the MIT license (see LICENSE.txt).

Building
======
CMake must be installed, at least version 3.14. Either use cmake graphical interface, or use the commandline.

Inside the project folder, e.g.
```bash
mkdir build
cd build
# Creates the build for the library and the test code.
cmake -D SignalTest=1 .. 
# Run the tests.
./SignalTest
```
