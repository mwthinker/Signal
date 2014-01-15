CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Signal)
if (MSVC)
else (MSVC)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
SET(SOURCES_EXTRA
	src/mw/signal.h
	src/mw/signals/connection.cpp
	src/mw/signals/connection.h
)

SET(SOURCES_EXTRA_TEST
	srcTest/main.cpp
)
# End of source files.

ADD_LIBRARY(Signal ${SOURCES_EXTRA})
INCLUDE_DIRECTORIES(src)
ADD_EXECUTABLE(SignalTest ${SOURCES_EXTRA_TEST})
TARGET_LINK_LIBRARIES(SignalTest Signal)
