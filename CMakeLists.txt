project(CCL)
cmake_minimum_required(VERSION 3.22)

add_executable(
	ccl
	src/ccl.cpp
	src/union_find.cpp
)

target_include_directories(
	ccl
PRIVATE
	include/	
)


