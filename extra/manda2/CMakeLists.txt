cmake_minimum_required(VERSION 3.17)
project(manda2 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(manda2
        memorynodemanager.h
        mymem.c
        mymem.h)
