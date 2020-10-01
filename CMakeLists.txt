cmake_minimum_required(VERSION 3.7...3.16)

project(
  Wenkenstein
  VERSION 0.1
  DESCRIPTION "A game engine made with raycasts and pain"
  LANGUAGES CXX)

add_definitions("-std=c++11")

include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(COPY font DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

macro(run_conan)
  # Download automatically, you can also just copy the conan.cmake file
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
  endif()
  include(${CMAKE_BINARY_DIR}/conan.cmake)
  conan_add_remote(
    NAME
    bincrafters
    URL
    https://api.bintray.com/conan/bincrafters/public-conan)
  conan_cmake_run(
    REQUIRES
    ${CONAN_EXTRA_REQUIRES}
    # -------------------------------------------------
    # Add Deps Here
    #--------------------------------------------------
    boost/1.74.0
    nlohmann_json/3.9.1
    # -------------------------------------------------
    # End Deps Here
    #--------------------------------------------------
    catch2/2.11.0
    fmt/6.2.0
    spdlog/1.5.0
    OPTIONS
    ${CONAN_EXTRA_OPTIONS}
    BASIC_SETUP
    CMAKE_TARGETS # individual targets to link to
    BUILD
    missing)
endmacro()

run_conan()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

#Set(Boost_DEBUG 1)
Set(Boost_INSTALL_DIR ${CONAN_BOOST_ROOT})
find_package(Boost 1.74.0 COMPONENTS REQUIRED program_options)

find_package(SDL2 REQUIRED)

find_package(SDL2_ttf REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${CONAN_INCLUDE_DIRS_NLOHMANN_JSON})
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::Main
                                              SDL2::TTF
                                              Boost::program_options
                                              CONAN_PKG::fmt
                                              CONAN_PKG::spdlog)

