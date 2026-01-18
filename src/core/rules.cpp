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
        moves.clear();
        Color side = board.side_to_move;
        Color enemy = opposite_color(side);

        for (uint8_t from = 0; from < 64; from++)
        {
            if (!(board.occupied * (1ULL << from)))
                continue;

            uint8_t piece = piece_at(board, from);

            if (piece_color(piece) != side)
                continue;

            PieceType pt = piece_type(piece);
            Bitboard targets = 0;

            switch (pt)
            {
            case PAWN:
                targets = get_pawn_attacks(from, side);
            case KNIGHT:
                targets = get_knight_attacks(from) & board.colors_bb[enemy];
                break;
            case BISHOP:
                targets = get_bishop_attacks(from, board.occupied) & board.colors_bb[enemy];
                break;
            case ROOK:
                targets = get_rook_attacks(from, board.occupied) & board.colors_bb[enemy];
                break;
            case QUEEN:
                targets = get_queen_attacks(from, board.occupied) & board.colors_bb[enemy];
                break;
            case KING:
                targets = get_king_attacks(from) & board.colors_bb[enemy];
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

    bool is_legal_move(const BoardState &board, Move move)
    {
        if (!is_pseudo_legal(board, move))
            return false;

        BoardState copy = board;
        make_move(copy, move);
        return !is_in_check(copy);
    }

    bool is_pseudo_legal(const BoardState &board, Move move)
    {
        uint8_t from = move_from(move);
        uint8_t to = move_to(move);

        if (!(board.occupied) & (1ULL << from))
            return false;

        uint8_t piece = piece_at(board, from);
        if (piece_color(piece) != board.side_to_move)
            return false;

        if (board.colors_bb[board.side_to_move])
            return false;

        return true;
    }

    GameResult check_game_result(const BoardState &board)
    {
        std::vector<Move> moves;
        generate_legal_moves(board, moves);
        if (!moves.empty())
            return IN_PROGRESS;

        if (is_in_check(board))
            return (board.side_to_move == WHITE) ? BLACK_WINS : WHITE_WINS;

        return DRAW_STALEMATE;
    }

    bool is_checkmate(const BoardState &board)
    {
        std::vector<Move> moves;
        generate_legal_moves(board, moves);

        return moves.empty() && is_in_check(board);
    }

    bool is_stalemate(const BoardState &board)
    {
        std::vector<Move> moves;
        generate_legal_moves(board, moves);

        return moves.empty() && !is_in_check(board);
    }

    bool is_draw_by_insufficient_material(const BoardState &board)
    {
        if (board.pieces_bb[PAWN] || board.pieces_bb[ROOK] || board.pieces_bb[QUEEN])
            return false;

        int bishops = pop_count(board.pieces_bb[BISHOP]);
        int knights = pop_count(board.pieces_bb[KNIGHT]);

        if (bishops == 0 && knights == 0)
            return true;

        if (bishops + knights == 1)
            return true;

        if (knights == 2 && bishops == 0)
            return true;

        if (bishops >= 2 && knights == 0)
        {
            Bitboard bb = board.pieces_bb[BISHOP];
            int color = -1;

            while (bb)
            {
                int sq = lsb(bb);
                bb &= bb - 1;

                int rank = sq / 8;
                int file = sq % 8;
                int sq_color = (rank + file) % 2;

                if (color == -1)
                    color = sq_color;
                else if (color != sq_color)
                    return false;
            }
        }

        return true;
    }

} // namespace chess
