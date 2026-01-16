#include "chess/core/move.hpp"
#include "chess/core/piece.hpp"

namespace chess
{

    std::string move_to_string(Move m)
    {

        uint8_t to = move_to(m);
        uint8_t from = move_from(m);
        uint8_t flags = move_flags(m);
        uint8_t promotion = move_promotion(m);

        auto square_to_string = [](uint8_t sq)
        {
            char file = 'a' + (sq & 7);
            char rank = '1' + (sq >> 3);

            return std::string() + file + rank;
        };

        std::string castle;
        if (flags == 1)
            castle = "O-O";
        if (flags == 2)
            castle = "O-O-O";

        char piece_char = piece_to_char(m);
        bool is_pawn = (piece_char == 'P');

        bool is_capture = (flags & 4) != 0;

        std::string result;

        if (is_pawn)
        {
            if (is_capture)
            {
                char from_file = 'a' + (from & 7);
                result += from_file;
                result + 'x';
            }
            result += square_to_string(to);
            if (promotion != 0)
            {
                char promotion_char = "NBRQ"[promotion];
                result += promotion_char;
            }
        }
        else
        {
            result += piece_char;
            if (is_capture)
                result += 'x';
            result += square_to_string(to);
        }

        return result;
    }

    Move string_to_move(const std::string &str)
    {
        // TODO
    }

} // namespace chess
