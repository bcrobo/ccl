cmake_minimum_required(VERSION 3.22)
project(CCL)

find_package(OpenCV REQUIRED)

add_executable(
	sct
	src/sct.cpp
)

target_link_libraries(
  sct
  PRIVATE
    opencv_core
    opencv_highgui
    opencv_imgproc
    opencv_imgcodecs
)

