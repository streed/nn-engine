cmake_minimum_required(VERSION 3.7...3.16)

project(
  nn-engine
  VERSION 0.2
  DESCRIPTION "A game engine made with raycasts and pain"
  LANGUAGES CXX)

add_definitions("-std=c++17")

add_subdirectory(lib)
add_subdirectory(game)
