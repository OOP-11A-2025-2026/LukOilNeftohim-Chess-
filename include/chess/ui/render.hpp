#ifndef CHESS_UI_RENDER_HPP
#define CHESS_UI_RENDER_HPP

#include "../core/board.hpp"
#include "../core/move.hpp"
#include <string>
#include <vector>

namespace chess {

struct RenderOptions {
    bool use_unicode = true;
    bool show_coordinates = true;
    bool flip_board = false;  // Show from black's perspective
    bool highlight_last_move = false;
    Move last_move = MOVE_NONE;
};

void render_board(const BoardState& board, const RenderOptions& options = RenderOptions());
void render_board_simple(const BoardState& board);
void render_move_list(const std::vector<Move>& moves, int columns = 4);

constexpr const char* UNICODE_PIECES[12] = {
    "♙", "♘", "♗", "♖", "♕", "♔",  // White
    "♟", "♞", "♝", "♜", "♛", "♚"   // Black
};

constexpr const char ASCII_PIECES[12] = {
    'P', 'N', 'B', 'R', 'Q', 'K',  // White
    'p', 'n', 'b', 'r', 'q', 'k'   // Black
};

// Utility
std::string square_to_string(uint8_t square);
uint8_t string_to_square(const std::string& str);

} // namespace chess

#endif
