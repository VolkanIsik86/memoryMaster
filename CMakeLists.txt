cmake_minimum_required(VERSION 3.16)
project(memoryMaster C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(memoryMaster
        mymem.c
        mymem.h

        )
