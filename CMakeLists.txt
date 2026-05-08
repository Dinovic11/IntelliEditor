cmake_minimum_required(VERSION 3.10)

project(IntelliEditor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configuration pour MinGW-w64
if(MINGW)
    set(CMAKE_C_COMPILER "gcc")
endif()

# Ajouter les répertoires include
include_directories(include)

# exécutable de test
add_executable(test_main src/main.c)