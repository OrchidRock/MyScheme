cmake_minimum_required(VERSION 3.4.3)

set(VERSION_STRING "1.0")

project(MyScheme)

include_directories(include/)

add_executable(${PROJECT_NAME} src/main.cpp src/env.cpp)
