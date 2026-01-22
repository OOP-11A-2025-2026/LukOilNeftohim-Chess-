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
)

# 3. Create the library target
add_library(chess_core ${CORE_SOURCES})

# 4. Build the actual game application
# Based on your prompt, ensure your main loop code is in apps/main.cpp
add_executable(chess_game apps/main.cpp) 

# 5. Link the app to your logic library
target_link_libraries(chess_game PRIVATE chess_core)