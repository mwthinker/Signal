cmake_minimum_required(VERSION 3.21)
project(Signal
	DESCRIPTION
		"A header-only Signals and Slots library"
	LANGUAGES
		CXX
	VERSION
		1.0.1
)

set(SIGNAL_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/src/mw/signal.h
)

add_library(Signal INTERFACE)
add_library(Signal::Signal ALIAS Signal)

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if (CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(Signal INTERFACE --coverage)
	target_link_options(Signal INTERFACE --coverage)
endif ()

target_include_directories(Signal
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:include/Signal>
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


# -------------------------------------------------------------------------
# Install
install(TARGETS Signal
	EXPORT SignalTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(DIRECTORY src/mw/
	DESTINATION include/Signal/mw
	FILES_MATCHING PATTERN "*.h"
)

include(CMakePackageConfigHelpers)

# Export the package configuration
install(EXPORT SignalTargets
	FILE SignalTargets.cmake
	NAMESPACE Signal::
	DESTINATION share/cmake/Signal
)

# Generate the package configuration file
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/SignalConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/SignalConfig.cmake
	INSTALL_DESTINATION share/cmake/Signal
)

# Install the generated package configuration file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SignalConfig.cmake
	DESTINATION share/cmake/Signal
)

# Install the package configuration file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/SignalConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SignalConfigVersion.cmake
	DESTINATION share/cmake/Signal
)

install(FILES LICENSE.txt DESTINATION share/doc/Signal)
