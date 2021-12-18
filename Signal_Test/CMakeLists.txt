cmake_minimum_required(VERSION 3.14)
project(Signal_Test
	DESCRIPTION
		"Test the project Signal using Catch2"
	LANGUAGES
		CXX
)

find_package(Catch2 CONFIG)
if (Catch2_FOUND)

	add_executable(Signal_Test
		src/main.cpp
		${SIGNAL_HEADERS}
	)

	target_link_libraries(Signal_Test 
		PRIVATE
			Signal
			Catch2::Catch2
	)

	if (MSVC)
		target_compile_options(Signal_Test
			PRIVATE
				"/permissive-"
		)
	endif (MSVC)

	set_target_properties(Signal_Test
		PROPERTIES
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED YES
			CXX_EXTENSIONS NO
	)

else (Catch2_FOUND)
	message(STATUS "Catch2 not found, Signal_Test not created")
endif (Catch2_FOUND)
