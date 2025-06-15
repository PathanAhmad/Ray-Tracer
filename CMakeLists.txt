# This is from YT tutorial
cmake_minimum_required(VERSION 3.10)
project(raytracer3a)

set(CMAKE_CXX_STANDARD 17)

add_executable(ray3a
    src/main.cpp        
    src/write_ppm.cpp   
    src/vec.h           
    src/ray.h           
)
