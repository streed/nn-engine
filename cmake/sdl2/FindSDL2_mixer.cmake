# FindSDL2_mixer.cmake
# Based on FindSDL2_ttf.cmake pattern

# SDL2 Library required
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
  set(SDL2_MIXER_SDL2_NOT_FOUND "Could NOT find SDL2 (SDL2 is required by SDL2_mixer).")
  if(SDL2_mixer_FIND_REQUIRED)
    message(FATAL_ERROR ${SDL2_MIXER_SDL2_NOT_FOUND})
  else()
    if(NOT SDL2_mixer_FIND_QUIETLY)
      message(STATUS ${SDL2_MIXER_SDL2_NOT_FOUND})
    endif()
    return()
  endif()
  unset(SDL2_MIXER_SDL2_NOT_FOUND)
endif()

set(SDL2_MIXER_PATH "" CACHE STRING "Custom SDL2_mixer Library path")

set(_SDL2_MIXER_NO_DEFAULT_PATH OFF)
if(SDL2_MIXER_PATH)
  set(_SDL2_MIXER_NO_DEFAULT_PATH ON)
endif()

set(SDL2_MIXER_NO_DEFAULT_PATH ${_SDL2_MIXER_NO_DEFAULT_PATH}
    CACHE BOOL "Disable search SDL2_mixer Library in default path")
unset(_SDL2_MIXER_NO_DEFAULT_PATH)

set(SDL2_MIXER_NO_DEFAULT_PATH_CMD)
if(SDL2_MIXER_NO_DEFAULT_PATH)
  set(SDL2_MIXER_NO_DEFAULT_PATH_CMD NO_DEFAULT_PATH)
endif()

find_path(SDL2_MIXER_INCLUDE_DIR SDL_mixer.h
  HINTS
    ENV SDL2MIXERDIR
    ENV SDL2DIR
    ${SDL2_MIXER_NO_DEFAULT_PATH_CMD}
  PATH_SUFFIXES SDL2
                include/SDL2 include
  PATHS ${SDL2_MIXER_PATH}
  DOC "Where the SDL2_mixer headers can be found"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VC_LIB_PATH_SUFFIX lib/x64)
else()
  set(VC_LIB_PATH_SUFFIX lib/x86)
endif()

find_library(SDL2_MIXER_LIBRARY
  NAMES SDL2_mixer
  HINTS
    ENV SDL2MIXERDIR
    ENV SDL2DIR
    ${SDL2_MIXER_NO_DEFAULT_PATH_CMD}
  PATH_SUFFIXES lib ${VC_LIB_PATH_SUFFIX}
  PATHS ${SDL2_MIXER_PATH}
  DOC "Where the SDL2_mixer Library can be found"
)

set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})
set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2_mixer
                                  REQUIRED_VARS SDL2_MIXER_LIBRARIES SDL2_MIXER_INCLUDE_DIRS)

mark_as_advanced(SDL2_MIXER_PATH
                 SDL2_MIXER_NO_DEFAULT_PATH
                 SDL2_MIXER_LIBRARY
                 SDL2_MIXER_INCLUDE_DIR)

if(SDL2_MIXER_FOUND)
  if(SDL2_MIXER_LIBRARY AND NOT TARGET SDL2::Mixer)
    add_library(SDL2::Mixer UNKNOWN IMPORTED)
    set_target_properties(SDL2::Mixer PROPERTIES
                          IMPORTED_LOCATION "${SDL2_MIXER_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${SDL2_MIXER_INCLUDE_DIR}"
                          INTERFACE_LINK_LIBRARIES SDL2::Core)
  endif()
endif()
