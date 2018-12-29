cmake_minimum_required(VERSION 3.10)
project(myShCommand)

set(CMAKE_C_STANDARD 11)

ADD_SUBDIRECTORY(include)
include_directories(./include)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)

add_executable(myls myls_opt.c)
TARGET_LINK_LIBRARIES(myls MYSHLIB)

# add_executable(myps myps.c)
#add_executable(mypwd mypwd.c)
