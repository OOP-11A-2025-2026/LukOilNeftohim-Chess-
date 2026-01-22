cmake_minimum_required(VERSION 3.10)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 17)

# 1. Include the 'include' directory so headers can be found
# This allows #include "chess/core/board.hpp" to work
include_directories(include)

# 2. Collect all core logic files
# We use the explicit paths shown in your project sidebar
set(CORE_SOURCES
    src/core/board.cpp
    src/core/move.cpp
    src/core/piece.cpp
    src/core/rules.cpp
    src/engine/eval.cpp
    src/engine/search.cpp
    src/engine/uci.cpp
    src/parser/fen.cpp
    src/parser/png.cpp
    src/parser/san.cpp
    src/storage/file_reader.cpp
    src/storage/file_writer.cpp
    src/storage/storage.cpp
    src/ui/input.cpp
    src/ui/render.cpp
)

# 3. Create the library target
add_library(chess_core ${CORE_SOURCES})

# 4. Build the actual game application
# Based on your prompt, ensure your main loop code is in apps/main.cpp
add_executable(chess_game apps/main.cpp) 

# 5. Link the app to your logic library
target_link_libraries(chess_game PRIVATE chess_core)

# 6. Testing Setup (Catch2 - using local amalgamated)
enable_testing()

# Create test executable with Catch2 amalgamated and test files
add_executable(chess_tests
    tests/catch2/catch_amalgamated.cpp
    tests/core/engine_test.cpp
    tests/engine/uci_test.cpp
    tests/storage/storage_test.cpp
)

# Catch2 amalgamated includes need to be available
target_include_directories(chess_tests PRIVATE tests/catch2)
target_link_libraries(chess_tests PRIVATE chess_core)