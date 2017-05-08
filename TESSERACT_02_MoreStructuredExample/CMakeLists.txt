cmake_minimum_required(VERSION 2.8.7)

message(STATUS "You should call cmake when you are in build/Debug or in build/Release. You should call 'cmake ../..'")
set(PROJECT_NAME "Tesseract")
project(${PROJECT_NAME})


#get the directory
get_filename_component(PARENTDIR ${CMAKE_BINARY_DIR} NAME)
message(STATUS "cmake is working in directory " ${PARENTDIR})
message(STATUS "cmake CMAKE_BINARY_DIR is " ${CMAKE_BINARY_DIR})  

if(PARENTDIR STREQUAL "Release")
    message(STATUS "Building Release!")
    #totally disable log.h
    set(CMAKE_BUILD_TYPE "Release")
endif(PARENTDIR STREQUAL "Release")

if(PARENTDIR STREQUAL "Debug")
    message(STATUS "Building Debug!")
    set(CMAKE_BUILD_TYPE "Debug")
    add_definitions(-Werror=implicit-function-declaration)
endif(PARENTDIR STREQUAL "Debug")

#we set which standard we want to use: in this way we ensure it's not the compiler to choose the standard
#added because turing gcc was different than mine
add_definitions(-std=c++11 -Wfatal-errors)

add_subdirectory(src/main/cpp)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
