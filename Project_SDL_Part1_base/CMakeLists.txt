cmake_minimum_required (VERSION 3.0)
project ("Project_SDL_sub")

IF(WIN32)
  message(STATUS "Building for windows")

  set(SDL2_INCLUDE_DIRS "SDL2/include")
  set(SDL2IMAGE_INCLUDE_DIRS "SDL2_image/include")

  # Support both 32 and 64 bit builds
  if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
    message(STATUS "Building for 64bit")
    set(SDL2_LINK_DIRS "SDL2/lib/x64")
    set(SDL2IMAGE_LINK_DIRS "SDL2_image/lib/x64")
  else ()
    message(STATUS "Building for 32bit")
    set(SDL2_LINK_DIRS "SDL2/lib/x86")
    set(SDL2IMAGE_LINK_DIRS "SDL2_image/lib/x86")
  endif ()

  include_directories(${SDL2_INCLUDE_DIRS})
  include_directories(${SDL2IMAGE_INCLUDE_DIRS})
  link_directories(${SDL2_LINK_DIRS}, ${SDL2IMAGE_LINK_DIRS})

  add_executable(SDL_part1 main.cpp Project_SDL1.cpp)
  target_link_libraries(SDL_part1 PUBLIC SDL2 SDL2main SDL2_image)
ELSE()
  message(STATUS "Building for Linux or Mac")
  set(CMAKE_CXX_STANDARD 20)

  add_executable(SDL_part1 main.cpp Project_SDL1.cpp)


  set(SDL2_LIB "/Library/Frameworks/SDL2.framework/SDL2")
  set(SDL2_image_LIB "/Library/Frameworks/SDL2_image.framework/SDL2_image")

  target_include_directories(SDL_part1 PRIVATE "/Library/Frameworks/SDL2.framework/Headers")
  target_include_directories(SDL_part1 PRIVATE "/Library/Frameworks/SDL2_image.framework/Headers")

  target_link_libraries(SDL_part1 ${SDL2_LIB} ${SDL2_image_LIB})

ENDIF()

