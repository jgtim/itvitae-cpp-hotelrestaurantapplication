# minimum version of cmake
cmake_minimum_required(VERSION 3.10)

# project name and language
project(resort-systeem LANGUAGES CXX)

# cpp language version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# get all sources
file(GLOB_RECURSE SOURCES "src/*.cpp")

# target executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# include own headers
target_include_directories(${CMAKE_PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# copy data/assets directory to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)