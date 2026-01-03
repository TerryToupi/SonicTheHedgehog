cmake_minimum_required(VERSION 3.20)
project(Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} 
    PRIVATE
        ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(box2d REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        SDL3::SDL3
        SDL3_mixer::SDL3_mixer
        box2d::box2d
        nlohmann_json::nlohmann_json
)

source_group(
    TREE 
        ${CMAKE_CURRENT_SOURCE_DIR} 
    PREFIX 
        "Engine" 
    FILES 
        ${SOURCES}
)