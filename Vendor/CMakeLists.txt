cmake_minimum_required(VERSION 3.20)
project(Vendor LANGUAGES C CXX)

include(FetchContent)

# global settings
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "")

FetchContent_Declare(SDL
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(SDL_mixer
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SDL_mixer
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(SDL_mixer)

FetchContent_Declare(nlohmann_json
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/json
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(nlohmann_json)

