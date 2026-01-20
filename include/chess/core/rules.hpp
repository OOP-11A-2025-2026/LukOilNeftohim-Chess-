#ifndef CHESS_CORE_RULES_HPP
#define CHESS_CORE_RULES_HPP

#include "board.hpp"
#include "move.hpp"
#include <vector>

namespace chess
{

    void generate_legal_moves(const BoardState &board, std::vector<Move> &moves);
    void generate_pseudo_legal_moves(const BoardState &board, std::vector<Move> &moves);
    void generate_captures(const BoardState &board, std::vector<Move> &moves);

    bool is_legal_move(const BoardState &board, Move move);
    bool is_pseudo_legal(const BoardState &board, Move move);

    enum GameResult
    {
        IN_PROGRESS,
        WHITE_WINS,
        BLACK_WINS,
        DRAW_STALEMATE,
        DRAW_INSUFFICIENT,
        DRAW_FIFTY_MOVE,
        DRAW_REPETITION
    };

    GameResult check_game_result(const BoardState &board);
    bool is_checkmate(const BoardState &board);
    bool is_stalemate(const BoardState &board);
    bool is_draw_by_insufficient_material(const BoardState &board);

} // namespace chess

#endif
