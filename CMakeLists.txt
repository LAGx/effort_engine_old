# by kvark 
# ready only for linux
# v0.3


cmake_minimum_required(VERSION 3.7)
project(effort_engine)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

#set(ENV{VULKAN_SDK} "/home/kvark/tools/VulkanSDK/1.0.65.0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#progect settings

#debug and release modes
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(">> enable DEBUG_MODE mode")
    add_definitions(-DDEBUG_MODE=1)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(">> enable RELEASE_MODE mode")
    add_definitions(-DRELEASE_MODE=0)
    add_definitions(-DNDEBUG)
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


include_directories("source/headers")
include_directories("/usr/local/include/lua")
include_directories("/usr/local/include/GLFW")
include_directories($ENV{VULKAN_SDK}/include)
include_directories("/usr/local/include/anvil")


add_executable(${PROJECT_NAME} ${ALL_CPP_NATIVE_CODE})


target_link_libraries (${PROJECT_NAME} glfw)
target_include_directories(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
#i don`t know why it need there
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/liblua53.a" ${CMAKE_DL_LIBS})

#anvil libs
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libAnvil.a" ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libglslang.a" ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libHLSL.a" ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libOGLCompiler.a" ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libOSDependent.a" ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libSPIRV.a" ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} "/usr/local/lib/anvil/libSPVRemapper.a" ${CMAKE_DL_LIBS})
