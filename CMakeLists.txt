cmake_minimum_required(VERSION 2.8)
project(Signal)
if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++1y")
endif ()

# Source files.
set(SOURCES_EXTRA
	src/mw/signal.h
	src/mw/signals/connection.cpp
	src/mw/signals/connection.h
)

set(SOURCES_EXTRA_TEST
	srcTest/main.cpp
)
# End of source files.

add_library(Signal ${SOURCES_EXTRA})

option(SignalTest "SignalTest project is added" OFF)

if (SignalTest)
	include_directories(src)
	add_executable(SignalTest ${SOURCES_EXTRA_TEST})
	target_link_libraries(SignalTest Signal)
endif (SignalTest)
