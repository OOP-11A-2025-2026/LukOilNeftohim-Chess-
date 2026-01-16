#include "chess/core/piece.hpp"

namespace chess
{

    char piece_to_char(uint8_t piece)
    {
        PieceType type = piece_type(piece);
        Color color = piece_color(piece);

        switch (type)
        {
        case PieceType::PAWN:
            return color == Color::WHITE ? 'P' : 'p';

        case PieceType::BISHOP:
            return color == Color::WHITE ? 'B' : 'b';

        case PieceType::KNIGHT:
            return color == Color::WHITE ? 'N' : 'n';

        case PieceType::ROOK:
            return color == Color::WHITE ? 'R' : 'r';

        case PieceType::QUEEN:
            return color == Color::WHITE ? 'Q' : 'q';

        case PieceType::KING:
            return color == Color::WHITE ? 'K' : 'k';

        default:
            return '?';
        }
    }

    uint8_t char_to_piece(char c)
    {
        switch (c)
        {
        case 'P':
            return make_piece(PieceType::PAWN, Color::WHITE);

        case 'B':
            return make_piece(PieceType::BISHOP, Color::WHITE);

        case 'N':
            return make_piece(PieceType::KNIGHT, Color::WHITE);

        case 'R':
            return make_piece(PieceType::ROOK, Color::WHITE);

        case 'Q':
            return make_piece(PieceType::QUEEN, Color::WHITE);

        case 'K':
            return make_piece(PieceType::KING, Color::WHITE);

        case 'p':
            return make_piece(PieceType::PAWN, Color::BLACK);

        case 'b':
            return make_piece(PieceType::BISHOP, Color::BLACK);

        case 'n':
            return make_piece(PieceType::KNIGHT, Color::BLACK);

        case 'r':
            return make_piece(PieceType::ROOK, Color::BLACK);

        case 'q':
            return make_piece(PieceType::QUEEN, Color::BLACK);

        case 'k':
            return make_piece(PieceType::KING, Color::BLACK);

        default:
            return 0;
        }
    }

} // namespace chess
