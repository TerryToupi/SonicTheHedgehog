cmake_minimum_required(VERSION 3.20)
project(Application LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(${PROJECT_NAME} 
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        ASSETS="${CMAKE_CURRENT_SOURCE_DIR}/Assets"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Engine
)

source_group(
    TREE 
        ${CMAKE_CURRENT_SOURCE_DIR} 
    PREFIX 
        "Application" 
    FILES 
        ${SOURCES}
)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME}
    )
endif()