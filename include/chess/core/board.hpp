#ifndef CHESS_CORE_BOARD_HPP
#define CHESS_CORE_BOARD_HPP

#include "piece.hpp"
#include "move.hpp"
#include <cstdint>
#include <array>

namespace chess {

using Bitboard = uint64_t;

extern const std::array<std::array<uint64_t, 64>, 12> ZOBRIST_PIECES;
extern const std::array<uint64_t, 16> ZOBRIST_CASTLING;
extern const std::array<uint64_t, 8> ZOBRIST_EN_PASSANT;
extern const uint64_t ZOBRIST_SIDE;

struct BoardState {
    std::array<Bitboard, 6> pieces_bb;  // Bitboards за всеки тип фигура
    std::array<Bitboard, 2> colors_bb;  // Bitboards за всеки цвят
    Bitboard occupied;                   // Всички фигури
    
    Color side_to_move;
    uint8_t castling_rights;  // 4 bits: KQkq
    uint8_t en_passant_file;  // 0-7 или 8 за none
    uint8_t halfmove_clock;
    uint16_t fullmove_number;
    
    uint64_t hash;  // Zobrist hash
    
    BoardState();
};

constexpr Bitboard square_bb(uint8_t sq) { return 1ULL << sq; }
constexpr bool test_bit(Bitboard bb, uint8_t sq) { return bb & square_bb(sq); }
constexpr Bitboard set_bit(Bitboard bb, uint8_t sq) { return bb | square_bb(sq); }
constexpr Bitboard clear_bit(Bitboard bb, uint8_t sq) { return bb & ~square_bb(sq); }

int pop_count(Bitboard bb);
int lsb(Bitboard bb);  // Least significant bit
int msb(Bitboard bb);  // Most significant bit
Bitboard pop_lsb(Bitboard& bb);  // Extract and clear LSB

void init_board(BoardState& board);
void reset_board(BoardState& board);
uint8_t piece_at(const BoardState& board, uint8_t square);
void place_piece(BoardState& board, uint8_t square, uint8_t piece);
void remove_piece(BoardState& board, uint8_t square);
void make_move(BoardState& board, Move move);
void unmake_move(BoardState& board, Move move);

Bitboard get_pawn_attacks(uint8_t square, Color color);
Bitboard get_knight_attacks(uint8_t square);
Bitboard get_king_attacks(uint8_t square);
Bitboard get_bishop_attacks(uint8_t square, Bitboard occupied);
Bitboard get_rook_attacks(uint8_t square, Bitboard occupied);
Bitboard get_queen_attacks(uint8_t square, Bitboard occupied);

bool is_square_attacked(const BoardState& board, uint8_t square, Color by_color);
bool is_in_check(const BoardState& board);
uint64_t compute_hash(const BoardState& board);

} // namespace chess

#endif
