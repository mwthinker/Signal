cmake_minimum_required(VERSION 3.14)
project(Signal_Example
	DESCRIPTION
		"An example of using the Signal library"
	LANGUAGES
		CXX
)

add_executable(Signal_Example
	src/main.cpp
)
	
target_link_libraries(Signal_Example
	PRIVATE
		Signal
)

set_target_properties(Signal_Example
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

if (MSVC)
	target_compile_options(Signal_Example
		PRIVATE
			"/permissive-"
	)
endif (MSVC)
