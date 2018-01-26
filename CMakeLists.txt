
cmake_minimum_required(VERSION 2.8)
project(effort_engine)

#progect settings

#debug and release modes
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(">> enable DEBUG_MODE mode")
    add_definitions(-DDEBUG_MODE=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(">> enable RELEASE_MODE mode")
    add_definitions(-DRELEASE_MODE=0)
else()
    message(FATAL_ERROR "flag -DCMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE} " is invalid") #try to set -DCMAKE_BUILD_TYPE=Release or -DCMAKE_BUILD_TYPE=Debug
endif()

#windows, linux, macOS
# WIN_OS LINUX_OS MAC_OS
message(">> selected " ${OS} " operating system")
if( NOT ((${OS} STREQUAL "WIN_OS") OR (${OS} STREQUAL "LINUX_OS") OR (${OS} STREQUAL "MAC_OS")))
    message( FATAL_ERROR "flag -DOS=" ${OS} " is invalid") #try to set -DOS=LINUX_OS or -DOS=WIN_OS or -DOS=MAC_OS
endif()
add_definitions(-D${OS})

#settings variables
file(GLOB_RECURSE ALL_CPP_NATIVE_CODE "source/scr/*.cpp")

#make settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("source/headers/")


add_executable(../${PROJECT_NAME} ${ALL_CPP_NATIVE_CODE})
