project(Signal_Test
	DESCRIPTION
		"Test the project Signal using GTest"
	LANGUAGES
		CXX
)

find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(Signal_Test
	src/signaltests.cpp
	src/scopedconnectiontests.cpp
	${SIGNAL_HEADERS}
)

target_link_libraries(Signal_Test
	PUBLIC
		Signal
		GTest::gtest GTest::gtest_main # Test explorer on Visual Studio 2022 will not find test if "GTest::gmock_main GTest::gmock" is added?
)

if (MSVC)
	target_compile_options(Signal_Test
		PRIVATE
			"/permissive-"
	)
endif ()

set_target_properties(Signal_Test
	PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED YES
		CXX_EXTENSIONS NO
)

include(GoogleTest)
gtest_discover_tests(Signal_Test)
