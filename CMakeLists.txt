cmake_minimum_required(VERSION 3.1...3.14)

set(VCPKG_ROOT $ENV{VCPKG_ROOT})
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
		CACHE STRING ""
	)
	message(STATUS "Use VCPKG CMAKE_TOOLCHAIN_FILE")
endif()

project(Signal
	DESCRIPTION
		"A simple Signals and Slots library"
	LANGUAGES
		CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SIGNAL_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/mw/signal.h
)

add_library(Signal INTERFACE)

target_include_directories(Signal
	INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

#add_custom_target(${PROJECT_NAME}_ SOURCES ${PROJECT_SOURCES})
message(STATUS "Signal_Test is available to add: -DSignal_Test=1")
option(Signal_Test "Signal_Test project is added" OFF)
if (Signal_Test)
	add_subdirectory(Signal_Test)
endif (Signal_Test)
