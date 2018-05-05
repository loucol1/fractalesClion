cmake_minimum_required(VERSION 3.10)
project(info_Clion C)

set(CMAKE_C_STANDARD 11)

include_directories(project_2018_template/libfractal)

add_executable(info_Clion
        project_2018_template/libfractal/fractal.c
        project_2018_template/libfractal/fractal.h
        project_2018_template/libfractal/tools.c
        project_2018_template/main.c)
