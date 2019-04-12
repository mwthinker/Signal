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

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/mw/signal.h
)

add_library(Signal INTERFACE)

target_include_directories(Signal
	INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

find_package(Catch2 CONFIG)
if (Catch2_FOUND)
	message(STATUS "SignalTest is available to add: -DSignalTest=1")
	option(SignalTest "SignalTest project is added" OFF)
	if (SignalTest)
		set(SOURCES_TEST
			srcTest/main.cpp
		)
	
		add_executable(SignalTest ${SOURCES_TEST} ${HEADERS})	
	
		target_link_libraries(SignalTest 
			PRIVATE
				Signal
				Catch2::Catch2
		)
	endif (SignalTest)
else (Catch2_FOUND)
	message(STATUS "Catch2 not found, SignalTest not created")
endif (Catch2_FOUND)
