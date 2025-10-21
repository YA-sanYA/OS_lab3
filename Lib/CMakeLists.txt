add_library(Lib STATIC
	Threads.h
	Threads.cpp
	utils.h)

target_include_directories(Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})