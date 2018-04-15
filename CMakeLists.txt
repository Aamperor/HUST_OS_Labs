cmake_minimum_required(VERSION 3.9)
project(OSlabs)


include(CMakeToolsHelpers OPTIONAL)
set(CMAKE_CXX_STANDARD 11)

#if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -lpthread")
#else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
#    set(CMAKE_BUILD_TYPE "Release")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -lpthread -fopenmp")
#endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )

find_package(Threads)
#add_executable(oslab2 oslab2.c)
#target_link_libraries(oslab2 ${CMAKE_THREAD_LIBS_INIT})


add_executable(oslab3 oslab3.c)
target_link_libraries(oslab3 ${CMAKE_THREAD_LIBS_INIT})
