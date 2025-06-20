cmake_minimum_required(VERSION 3.31)
project(opencv_playground)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)

add_executable(opencv_playground main.cpp)
