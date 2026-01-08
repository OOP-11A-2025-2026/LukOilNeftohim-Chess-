#ifndef CHESS_ENGINE_EVAL_HPP
#define CHESS_ENGINE_EVAL_HPP

#include "../core/board.hpp"
#include <cstdint>

namespace chess {

constexpr int32_t EVAL_INFINITY = 100000;
constexpr int32_t EVAL_CHECKMATE = 50000;

constexpr int32_t PIECE_VALUES[6] = {
    100,   // Pawn
    320,   // Knight
    330,   // Bishop
    500,   // Rook
    900,   // Queen
    20000  // King
};

int32_t evaluate(const BoardState& board);
int32_t evaluate_material(const BoardState& board);
int32_t evaluate_position(const BoardState& board);
int32_t evaluate_mobility(const BoardState& board);
int32_t evaluate_king_safety(const BoardState& board);
int32_t evaluate_pawn_structure(const BoardState& board);

extern const std::array<std::array<int32_t, 64>, 6> PST_MG;  // Middlegame
extern const std::array<std::array<int32_t, 64>, 6> PST_EG;  // Endgame

int get_game_phase(const BoardState& board);

} // namespace chess

#endif
