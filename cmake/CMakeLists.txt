cmake_minimum_required(VERSION 3.8)


project(testcpp LANGUAGES CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#set(CMAKE_CXX_COMPILER "g++-7")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(MAINOBJ ../src/main.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../out)

file(GLOB_RECURSE SOURCES ../src/*.cpp ../src/*.h )

add_executable(${PROJECT_NAME} ${MAINOBJ} ${SOURCES})

