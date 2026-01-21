#ifndef CHESS_CORE_MOVE_HPP
#define CHESS_CORE_MOVE_HPP

#include <cstdint>
#include <string>

namespace chess
{
    // Forward declaration
    struct BoardState;

    // Bits 0-5: from square (0-63)
    // Bits 6-11: to square (0-63)
    // Bits 12-13: promotion piece type (0-3 mapped to N,B,R,Q)
    // Bits 14-15: special move flags (castle, en passant, etc)
    struct BoardState;

    using Move = uint16_t;

    constexpr uint16_t MOVE_NONE = 0;

    constexpr Move make_move(uint8_t from, uint8_t to)
    {
        return static_cast<Move>((to << 6) | from);
    }

    constexpr Move make_promotion(uint8_t from, uint8_t to, uint8_t promo_type)
    {
        return static_cast<Move>((promo_type << 12) | (to << 6) | from);
    }

    constexpr uint8_t move_from(Move m) { return m & 0x3F; }
    constexpr uint8_t move_to(Move m) { return (m >> 6) & 0x3F; }
    constexpr uint8_t move_promotion(Move m) { return (m >> 12) & 0x7; }
    constexpr uint8_t move_flags(Move m) { return (m >> 14) & 0x3; }

    int square_from_str(const std::string &sq);
    std::string square_to_str(int sq);

} // namespace chess

#endif
