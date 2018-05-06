cmake_minimum_required(VERSION 3.10)
project(project_2018_template C)

set(CMAKE_C_STANDARD 99)

include_directories(libfractal)

add_executable(project_2018_template
        libfractal/fractal.c
        libfractal/fractal.h
        main.c libfractal/tools.c)
