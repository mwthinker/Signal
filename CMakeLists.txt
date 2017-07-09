cmake_minimum_required(VERSION 2.8)
project(Signal)
if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++1y")
endif ()

# Source files.
set(SOURCES
	src/mw/signal.h
)

set(SOURCES_TEST
	srcTest/main.cpp
)
# End of source files.

add_library(Signal INTERFACE)

target_include_directories(Signal INTERFACE ${SOURCES})

option(SignalTest "SignalTest project is added" OFF)

if (SignalTest)
	include_directories(src/)
	add_executable(SignalTest ${SOURCES_TEST} ${SOURCES})
	target_link_libraries(SignalTest Signal)
endif (SignalTest)
