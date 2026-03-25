cmake_minimum_required(VERSION 3.7...3.16)

project(
  Wenkenstein
  VERSION 0.3
  DESCRIPTION "A game engine made with raycasts and pain"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(COPY ../font DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ../textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ../game.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ../maps DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ../tilesets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
file(COPY ../sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

add_executable(${PROJECT_NAME} ${SOURCES})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/sdl2)

find_package(Boost COMPONENTS REQUIRED program_options filesystem)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/lib/include)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Main
                                              SDL2::TTF
                                              SDL2::Mixer
                                              Boost::program_options
                                              Boost::filesystem
                                              nnEngine)
