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
add_library(formatter STATIC src/editor/formatter.c)
add_library(exporter STATIC src/editor/exporter.c)
add_library(search_replace STATIC src/editor/search_replace.c)
add_library(memory STATIC src/utils/memory.c)
add_library(encoding STATIC src/utils/encoding.c)
add_library(config STATIC src/utils/config.c)

# Decoupled API and NLP stub for Dev B (UI) and Dev C (NLP)
add_library(editor_api STATIC src/editor_api.c)
add_library(nlp_stub STATIC src/nlp_stub.c)

# Ensure editor_api links its internal dependencies so other targets only need editor_api
target_link_libraries(editor_api PRIVATE gap_buffer exporter search_replace memory encoding config)

# Win32 UI prototype for Dev B
add_executable(win32_demo src/ui_win32.c)
target_link_libraries(win32_demo PRIVATE editor_api user32)

# Unit tests for editor_api
add_executable(test_editor_api tests/test_editor_api.c)
target_link_libraries(test_editor_api PRIVATE editor_api)

# exécutable de test
add_executable(main src/main.c)

target_link_libraries(main PRIVATE gap_buffer undo_redo formatter exporter search_replace memory encoding config editor_api nlp_stub)
