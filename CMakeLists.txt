cmake_minimum_required(VERSION 3.1...3.14)

set(VCPKG_ROOT $ENV{VCPKG_ROOT})
if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
		CACHE STRING ""
	)
	message(STATUS "Use VCPKG CMAKE_TOOLCHAIN_FILE")
endif ()

project(Signal
	DESCRIPTION
		"A simple Signals and Slots library"
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

target_compile_features(Signal INTERFACE cxx_std_17)

message(STATUS "Signal_Test is available to add: -DSignal_Test=1")
option(Signal_Test "Signal_Test project is added" OFF)
if (Signal_Test)
	add_subdirectory(Signal_Test)
endif (Signal_Test)
