# This is from YT tutorial
cmake_minimum_required(VERSION 3.10)
project(raytracer3a)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS")

# Add tinyxml2
add_library(tinyxml2 STATIC 
    ${CMAKE_SOURCE_DIR}/third_party/tinyxml2/tinyxml2.cpp
)

target_include_directories(tinyxml2 PUBLIC
    ${CMAKE_SOURCE_DIR}/third_party/tinyxml2
)

# Add source files
add_executable(ray3a
    src/main.cpp        
    src/scene.cpp
    src/scene_parser.cpp
    src/write_ppm.cpp
    src/material.cpp
    src/stb_image_write_impl.cpp
    src/stb_image_impl.cpp
    src/obj_utils.cpp
)

target_include_directories(ray3a PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party
)

target_link_libraries(ray3a tinyxml2)
