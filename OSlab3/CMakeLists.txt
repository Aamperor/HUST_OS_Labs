cmake_minimum_required(VERSION 3.9)
project(OSlab3)

include(CMakeToolsHelpers OPTIONAL)
set(CMAKE_CXX_STANDARD 11)

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -lpthread")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -lpthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )



find_package(Threads)


add_executable(oslab3_testcache oslab3_testcache.c)
target_link_libraries(oslab3_testcache.c ${CMAKE_THREAD_LIBS_INIT})
