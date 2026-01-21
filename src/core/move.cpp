#include "chess/core/move.hpp"
#include "chess/core/board.hpp"
#include "chess/core/rules.hpp"
#include <vector>

namespace chess
{

    int square_from_str(const std::string &sq)
    {
        if (sq.length() < 2)
            return -1;
        return (sq[1] - '1') * 8 + (sq[0] - 'a');
    }

    std::string square_to_str(int sq)
    {
        if (sq < 0 || sq > 63)
            return "??";
        char file = 'a' + (sq % 8);
        char rank = '1' + (sq / 8);
        return std::string{file, rank};
    }

    std::string move_to_string(const BoardState &, Move m)
    {
        if (m == 0)
            return "0000";

        int from = move_from(m);
        int to = move_to(m);

        auto to_algebraic = [](int sq)
        {
            char file = 'a' + (sq % 8);
            char rank = '1' + (sq / 8);
            return std::string{file, rank};
        };

        std::string s = to_algebraic(from) + to_algebraic(to);

        uint8_t promo = move_promotion(m);
        if (promo != 0)
        {
            switch (promo)
            {
            case KNIGHT:
                s += 'n';
                break;
            case BISHOP:
                s += 'b';
                break;
            case ROOK:
                s += 'r';
                break;
            case QUEEN:
                s += 'q';
                break;
            default:
                break;
            }
        }

        return s;
    }

    Move string_to_move(const BoardState &board, const std::string &str)
    {
        if (str.length() < 4)
            return 0;

        int from = square_from_str(str.substr(0, 2));
        int to = square_from_str(str.substr(2, 2));

        uint8_t promo_type = 0;
        if (str.length() == 5)
        {
            char p = str[4];
            if (p == 'n')
                promo_type = KNIGHT;
            else if (p == 'b')
                promo_type = BISHOP;
            else if (p == 'r')
                promo_type = ROOK;
            else if (p == 'q')
                promo_type = QUEEN;
        }

        std::vector<Move> moves;
        generate_legal_moves(board, moves);

        for (const Move &m : moves)
        {
            if (move_from(m) == from && move_to(m) == to)
            {
                if (promo_type != 0 && move_promotion(m) != promo_type)
                    continue;
                return m;
            }
        }
        return 0;
    }

} // namespace chess