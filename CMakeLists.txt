cmake_minimum_required(VERSION 3.1...3.16)

project(
  Wenkenstein
  VERSION 0.1
  DESCRIPTION "A game engine made with raycasts and pain"
  LANGUAGES CXX)


include_directories(include)
file(GLOB SOURCES "src/*.cpp")
file(COPY font DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED SDL2)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES})

