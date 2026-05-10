cmake_minimum_required(VERSION 3.10)

project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configuration pour MinGW-w64
if(MINGW)
    set(CMAKE_C_COMPILER "gcc")
endif()

# Ajout des répertoires include
include_directories(${CMAKE_SOURCE_DIR}/include)

# Bibliothèque interne de l'éditeur
add_library(gap_buffer STATIC src/editor/gap_buffer.c)
add_library(undo_redo STATIC src/editor/undo_redo.c)
add_library(encoding STATIC src/utils/encoding.c)

# exécutable de test
add_executable(main src/main.c)

target_link_libraries(main PRIVATE gap_buffer undo_redo encoding)
