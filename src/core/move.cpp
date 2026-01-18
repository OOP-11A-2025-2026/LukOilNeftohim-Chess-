#include "chess/core/move.hpp"
#include "chess/core/piece.hpp"
#include "chess/core/board.hpp"
#include "chess/core/rules.hpp"

#include <vector>
#include <iostream>

namespace chess
{

    std::string move_to_string(const BoardState &board, Move m)
    {

        uint8_t to = move_to(m);
        uint8_t from = move_from(m);
        uint8_t flags = move_flags(m);
        uint8_t promotion = move_promotion(m);

        uint8_t piece = piece_at(board, from);
        PieceType pt = piece_type(piece);

        auto sq = [](uint8_t square)
        {
            char file = 'a' + (square & 7);
            char rank = '1' + (square >> 3);

            return std::string() + file + rank;
        };

        if (pt == KING && abs((from & 7) - (to & 7)) == 2)
            return (to > from) ? "O-O" : "O-O-O";

        bool is_capture = piece_at(board, to) != make_piece(NONE, WHITE);

        std::string result;

        if (pt == PAWN)
        {
            if (is_capture)
            {
                result += char('a' + (from & 7));
                result += 'x';
            }
            result += sq(to);

            if (promotion)
                result += "NBRQ"[promotion];
        }
        else
        {
            result += piece_to_char(piece);
            if (is_capture)
                result += 'x';
            result += sq(to);
        }
        return result;
    }

    Move string_to_move(const BoardState &board, const std::string &str)
    {
        std::vector<Move> moves;
        generate_legal_moves(board, moves);

        for (const Move &m : moves)
        {
            if (move_to_string(board, m) == str)
                return m;
        }
        return 0;
    }

} // namespace chess
