cmake_minimum_required(VERSION 3.21)
project(Signal_Test
	DESCRIPTION
		"Test the project Signal using Catch2"
	LANGUAGES
		CXX
)

find_package(GTest CONFIG REQUIRED)
if (GTest_FOUND)
	enable_testing()

	add_executable(Signal_Test
		src/main.cpp
		src/scopedconnectiontests.cpp
		${SIGNAL_HEADERS}
	)

	target_link_libraries(Signal_Test 
		PRIVATE
			Signal
			GTest::gtest GTest::gtest_main # GTest::gmock_main GTest::gmock 
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
	
	include(GoogleTest)
	gtest_discover_tests(Signal_Test
		#WORKING_DIRECTORY "src/"
			
	)
	#add_test(NAME example_test COMMAND example)

else (GTest_FOUND)
	message(STATUS "GTest not found, Signal_Test not created")
endif (GTest_FOUND)
