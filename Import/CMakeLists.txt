cmake_minimum_required(VERSION 3.20)
project(Imports LANGUAGES C CXX)

# global settings
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "")

# CPM settings
set(CPM_USE_NAMED_CACHE_DIRECTORIES ON CACHE BOOL "")
set(CPM_SOURCE_CACHE ${CMAKE_CURRENT_SOURCE_DIR})

include(../cmake/CPM.cmake)

# SDL3
CPMAddPackage(
    NAME SDL 
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG main
)

# SDL3_mixer
CPMAddPackage(
    NAME SDL_mixer  
    GITHUB_REPOSITORY libsdl-org/SDL_mixer 
    GIT_TAG main
)

# Json 
CPMAddPackage(
    NAME nlohmann_json 
    GITHUB_REPOSITORY nlohmann/json 
    GIT_TAG develop 
)

# Box2D
CPMAddPackage(
    NAME box2d  
    GITHUB_REPOSITORY erincatto/box2d
    GIT_TAG main
)