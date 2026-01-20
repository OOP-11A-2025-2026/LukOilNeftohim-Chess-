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
            {
                int dir = (side_to_move == WHITE) ? 8 : -8;
                int start_rank = (side_to_move == WHITE) ? 1 : 6;

                uint8_t to = from + dir;
                if (to < 64 && !(board.occupied & (1ULL << to)))
                {
                    targets |= 1ULL << to;

                    if ((from / 8) == start_rank)
                    {
                        uint8_t to2 = from + 2 * dir;
                        if (!(board.occupied & (1ULL << to2)))
                            targets |= 1ULL << to2;
                    }
                }

                targets |= get_pawn_attacks(from, side_to_move) & board.colors_bb[opposite_color(side_to_move)];
                break;
            }
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
            if (!(board.occupied & (1ULL << from)))
                continue;

            uint8_t piece = piece_at(board, from);

            if (piece_color(piece) != side)
                continue;

            PieceType pt = piece_type(piece);
            Bitboard targets = 0;

            switch (pt)
            {
            case PAWN:
                targets = get_pawn_attacks(from, side) & board.colors_bb[enemy];
                break;
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

        Color mover = board.side_to_move;

        make_move(copy, move);

        copy.side_to_move = mover;

        return !is_in_check(copy);
    }

    bool is_pseudo_legal(const BoardState &board, Move move)
    {
        uint8_t from = move_from(move);
        uint8_t to = move_to(move);

        if (!(board.occupied & (1ULL << from)))
            return false;

        uint8_t piece = piece_at(board, from);
        if (piece_color(piece) != board.side_to_move)
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

        int white_bishops = pop_count(board.pieces_bb[BISHOP] & board.colors_bb[WHITE]);
        int black_bishops = pop_count(board.pieces_bb[BISHOP] & board.colors_bb[BLACK]);
        int white_knights = pop_count(board.pieces_bb[KNIGHT] & board.colors_bb[WHITE]);
        int black_knights = pop_count(board.pieces_bb[KNIGHT] & board.colors_bb[BLACK]);

        if (white_bishops + black_bishops + white_knights + black_knights == 0)
            return true;

        if (white_bishops + white_knights == 1 && black_bishops + black_knights == 0)
            return true;
        if (black_bishops + black_knights == 1 && white_bishops + white_knights == 0)
            return true;

        if ((white_knights == 2 && black_bishops + black_knights == 0) ||
            (black_knights == 2 && white_bishops + white_knights == 0))
            return true;

        auto all_bishops_same_color = [](Bitboard bb) -> bool
        {
            if (!bb)
                return true;
            int color = -1;
            while (bb)
            {
                int sq = lsb(bb);
                bb &= bb - 1;
                int sq_color = ((sq / 8) + (sq % 8)) % 2;
                if (color == -1)
                    color = sq_color;
                else if (color != sq_color)
                    return false;
            }
            return true;
        };

        Bitboard white_bishops_bb = board.pieces_bb[BISHOP] & board.colors_bb[WHITE];
        Bitboard black_bishops_bb = board.pieces_bb[BISHOP] & board.colors_bb[BLACK];

        if ((white_bishops >= 2 && !all_bishops_same_color(white_bishops_bb)) ||
            (black_bishops >= 2 && !all_bishops_same_color(black_bishops_bb)))
        {
            return false;
        }

        return true;
    }

} // namespace chess
