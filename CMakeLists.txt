cmake_minimum_required (VERSION 3.0)

project ("Project_SDL1")

set (CMAKE_CXX_STANDARD 20)

# Schließen Sie Unterprojekte ein.
add_subdirectory ("Project_SDL_Part1_base")

file(COPY media DESTINATION ${CMAKE_BINARY_DIR}/)
