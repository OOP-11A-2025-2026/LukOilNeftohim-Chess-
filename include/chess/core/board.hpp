#ifndef CHESS_CORE_BOARD_HPP
#define CHESS_CORE_BOARD_HPP

#include "piece.hpp"
#include "move.hpp"
#include <cstdint>
#include <array>
#include <optional>

namespace chess
{

    using Bitboard = uint64_t;

    extern const std::array<std::array<uint64_t, 64>, 12> ZOBRIST_PIECES;
    extern const std::array<uint64_t, 16> ZOBRIST_CASTLING;
    extern const std::array<uint64_t, 8> ZOBRIST_EN_PASSANT;
    extern const uint64_t ZOBRIST_SIDE;

    struct UndoInfo
    {
        Move move;
        uint8_t captured_piece;
        uint8_t castling_rights;
        uint8_t en_passant_file;
        uint8_t halfmove_clock;
        uint64_t hash;
    };

    constexpr int MAX_MOVES = 1024;

    struct MoveStack
    {
        UndoInfo stack[MAX_MOVES];
        int top = -1;

        void push(const UndoInfo &info)
        {
            stack[++top] = info;
        }

        UndoInfo pop()
        {
            return stack[top--];
        }

        bool empty() const
        {
            return top == -1;
        }
    };

    struct BoardState
    {
        std::array<Bitboard, 6> pieces_bb; // Bitboards за всеки тип фигура
        std::array<Bitboard, 2> colors_bb; // Bitboards за всеки цвят
        Bitboard occupied;                 // Всички фигури

        Color side_to_move;
        uint8_t castling_rights; // 4 bits: KQkq
        uint8_t en_passant_file; // 0-7 или 8 за none
        uint8_t halfmove_clock;
        uint16_t fullmove_number;

        uint64_t hash; // Zobrist hash

        MoveStack move_stack;

        BoardState();
    };

    constexpr uint8_t CASTLE_WHITE_KING = 1 << 0;
    constexpr uint8_t CASTLE_WHITE_QUEEN = 1 << 1;
    constexpr uint8_t CASTLE_BLACK_KING = 1 << 2;
    constexpr uint8_t CASTLE_BLACK_QUEEN = 1 << 3;

    constexpr Bitboard square_bb(uint8_t sq) { return 1ULL << sq; }
    constexpr bool test_bit(Bitboard bb, uint8_t sq) { return bb & square_bb(sq); }
    constexpr Bitboard set_bit(Bitboard bb, uint8_t sq) { return bb | square_bb(sq); }
    constexpr Bitboard clear_bit(Bitboard bb, uint8_t sq) { return bb & ~square_bb(sq); }

    int pop_count(Bitboard bb);
    int lsb(Bitboard bb);      // Least significant bit
    int msb(Bitboard bb);      // Most significant bit
    int pop_lsb(Bitboard &bb); // Extract and clear LSB

    void init_board(BoardState &board);
    void reset_board(BoardState &board);
    void set_starting_position(BoardState &board);
    uint8_t piece_at(const BoardState &board, uint8_t square);
    void place_piece(BoardState &board, uint8_t square, uint8_t piece);
    void remove_piece(BoardState &board, uint8_t square);
    void make_move(BoardState &board, Move move);
    void unmake_move(BoardState &board, Move move);

    Bitboard get_pawn_attacks(uint8_t square, Color color);
    Bitboard get_knight_attacks(uint8_t square);
    Bitboard get_king_attacks(uint8_t square);
    Bitboard get_bishop_attacks(uint8_t square, Bitboard occupied);
    Bitboard get_rook_attacks(uint8_t square, Bitboard occupied);
    Bitboard get_queen_attacks(uint8_t square, Bitboard occupied);

    bool is_square_attacked(const BoardState &board, uint8_t square, Color by_color);
    bool is_in_check(const BoardState &board);
    uint64_t compute_hash(const BoardState &board);

    std::string move_to_string(const BoardState &board, Move m);
    Move string_to_move(const BoardState &board, const std::string &str);

} // namespace chess

#endif
