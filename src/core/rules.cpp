#include "chess/core/rules.hpp"
#include <iostream>
#include <vector>

namespace chess
{

    void generate_legal_moves(const BoardState &board, std::vector<Move> &moves)
    {
        std::vector<Move> pseudo_moves;

        generate_pseudo_legal_moves(board, pseudo_moves);
        moves.clear();
        for (const Move &move : pseudo_moves)
        {
            if (is_legal_move(board, move))
            {
                moves.push_back(move);
            }
        }
    }

    void generate_pseudo_legal_moves(const BoardState &board, std::vector<Move> &moves)
    {
        // trqq se opravi
        moves.clear();
        Color side_to_move = board.side_to_move;

        for (uint8_t from = 0; from < 64; from++)
        {
            if (!(board.occupied & (1ULL << from)))
                continue;

            uint8_t piece = piece_at(board, from);
            if (piece_color(piece) != side_to_move)
                continue;

            PieceType pt = piece_type(piece);
            Bitboard targets = 0;

            switch (pt)
            {
            case PAWN:
                targets = get_pawn_attacks(from, side_to_move) & ~board.colors_bb[side_to_move];
                break;
            case KNIGHT:
                targets = get_knight_attacks(from) & ~board.colors_bb[side_to_move];
                break;
            case BISHOP:
                targets = get_bishop_attacks(from, board.occupied) & ~board.colors_bb[side_to_move];
                break;
            case ROOK:
                targets = get_rook_attacks(from, board.occupied) & ~board.colors_bb[side_to_move];
                break;
            case QUEEN:
                targets = get_queen_attacks(from, board.occupied) & ~board.colors_bb[side_to_move];
                break;
            case KING:
                targets = get_king_attacks(from) & ~board.colors_bb[side_to_move];
                break;
            default:
                break;
            }

            while (targets)
            {
                uint8_t to = lsb(targets);
                targets &= targets - 1;
                moves.push_back(make_move(from, to));
            }
        }
    }

    void generate_captures(const BoardState &board, std::vector<Move> &moves)
    {
        // TODO: Implement capture generation
    }

    bool is_legal_move(const BoardState &board, Move move)
    {
        // TODO: Implement legal move validation
        return true;
    }

    bool is_pseudo_legal(const BoardState &board, Move move)
    {
        // TODO: Implement pseudo-legal validation
        return true;
    }

    GameResult check_game_result(const BoardState &board)
    {
        // TODO: Implement game result checking
        return IN_PROGRESS;
    }

    bool is_checkmate(const BoardState &board)
    {
        // TODO: Implement checkmate detection
        return false;
    }

    bool is_stalemate(const BoardState &board)
    {
        // TODO: Implement stalemate detection
        return false;
    }

    bool is_draw_by_insufficient_material(const BoardState &board)
    {
        // TODO: Implement insufficient material detection
        return false;
    }

} // namespace chess
