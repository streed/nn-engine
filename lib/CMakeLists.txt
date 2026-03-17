cmake_minimum_required(VERSION 3.7...3.16)

project(
  nnEngine
  VERSION 0.2
  DESCRIPTION "A game engine made with raycasts and pain"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} ${SOURCES})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/sdl2)

find_package(Boost COMPONENTS REQUIRED program_options filesystem)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(nlohmann_json 3.9 REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Main
                                              SDL2::TTF
                                              Boost::program_options
                                              Boost::filesystem
                                              nlohmann_json::nlohmann_json)

install(TARGETS nnEngine LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
