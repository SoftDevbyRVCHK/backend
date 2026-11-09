cmake_minimum_required(VERSION 4.5)

project("odyssey framework" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    "framework" 
    
    SHARED
    src/core.cpp
)


target_compile_options("framework" PRIVATE -Wall -Wextra -Werror)

if(WIN32)
    target_link_libraries("framework" PUBLIC ws2_32 mswsock)
endif()

target_include_directories(
    "framework"
    
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        "C:/C/libs/boost_1_92_0"
)