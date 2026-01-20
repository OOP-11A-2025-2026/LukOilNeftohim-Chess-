#ifndef CHESS_CORE_PIECE_HPP
#define CHESS_CORE_PIECE_HPP

#include <cstdint>

namespace chess
{

    enum PieceType : uint8_t
    {
        PAWN = 0,
        KNIGHT = 1,
        BISHOP = 2,
        ROOK = 3,
        QUEEN = 4,
        KING = 5,
        NONE = 6
    };

    enum Color : uint8_t
    {
        WHITE = 0,
        BLACK = 1
    };

    constexpr Color opposite_color(Color c) { return static_cast<Color>(c ^ 1); }
    constexpr uint8_t make_piece(PieceType type, Color color) { return (color << 3) | type; }
    constexpr PieceType piece_type(uint8_t piece) { return static_cast<PieceType>(piece & 7); }
    constexpr Color piece_color(uint8_t piece) { return static_cast<Color>(piece >> 3); }

    char piece_to_char(uint8_t piece);
    uint8_t char_to_piece(char c);

} // namespace chess

#endif
