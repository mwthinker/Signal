cmake_minimum_required(VERSION 3.1...3.12)
project(Signal
	DESCRIPTION
		"A simple Signals and Slots library"
	LANGUAGES
		CXX
)
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/
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

option(SignalTest "SignalTest project is added" OFF)

if (SignalTest)
	set(SOURCES_TEST
		srcTest/main.cpp
	)
	
	add_executable(SignalTest ${HEADERS} ${SOURCES_TEST})

	target_include_directories(SignalTest
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/srcTest
	)
	
	target_link_libraries(SignalTest PRIVATE Signal)
endif (SignalTest)
