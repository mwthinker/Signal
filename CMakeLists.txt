cmake_minimum_required(VERSION 3.21)
project(Signal
	DESCRIPTION
		"A header-only Signals and Slots library"
	LANGUAGES
		CXX
)

set(SIGNAL_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/mw/signal.h
)

add_library(Signal INTERFACE)

target_include_directories(Signal
	INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

target_compile_features(Signal INTERFACE cxx_std_20)

message(STATUS "Signal_Test is available to add: -DSignal_Test=1")
option(Signal_Test "Signal_Test project is added" OFF)
if (Signal_Test)
	add_subdirectory(Signal_Test)
endif ()

message(STATUS "Signal_Example is available to add: -DSignal_Example=1")
option(Signal_Example "Add Signal_Example project." OFF)
if (Signal_Example)
	add_subdirectory(Signal_Example)
endif ()
