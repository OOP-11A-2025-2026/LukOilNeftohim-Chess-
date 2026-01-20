#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include "chess/core/board.hpp"
#include "chess/core/move.hpp"
#include "chess/core/piece.hpp"
#include "chess/core/rules.hpp"

#include <vector>
#include <iostream>

using namespace chess;

namespace squares
{
    constexpr uint8_t a1 = 0, b1 = 1, c1 = 2, d1 = 3, e1 = 4, f1 = 5, g1 = 6, h1 = 7;
    constexpr uint8_t a2 = 8, b2 = 9, c2 = 10, d2 = 11, e2 = 12, f2 = 13, g2 = 14, h2 = 15;
    constexpr uint8_t a3 = 16, b3 = 17, c3 = 18, d3 = 19, e3 = 20, f3 = 21, g3 = 22, h3 = 23;
    constexpr uint8_t a4 = 24, b4 = 25, c4 = 26, d4 = 27, e4 = 28, f4 = 29, g4 = 30, h4 = 31;
    constexpr uint8_t a5 = 32, b5 = 33, c5 = 34, d5 = 35, e5 = 36, f5 = 37, g5 = 38, h5 = 39;
    constexpr uint8_t a6 = 40, b6 = 41, c6 = 42, d6 = 43, e6 = 44, f6 = 45, g6 = 46, h6 = 47;
    constexpr uint8_t a7 = 48, b7 = 49, c7 = 50, d7 = 51, e7 = 52, f7 = 53, g7 = 54, h7 = 55;
    constexpr uint8_t a8 = 56, b8 = 57, c8 = 58, d8 = 59, e8 = 60, f8 = 61, g8 = 62, h8 = 63;
}
using namespace squares;

TEST_CASE("Bitboard helpers")
{
    Bitboard bb = 0b101010;
    REQUIRE(pop_count(bb) == 3);

    Bitboard single = 1ULL << 5;
    REQUIRE(lsb(single) == 5);
    REQUIRE(msb(single) == 5);

    Bitboard multi = 0b10010;
    int sq = pop_lsb(multi);
    REQUIRE(sq == 1);
    REQUIRE(multi == 0b10000);
}

TEST_CASE("Basic board setup")
{
    BoardState board;
    init_board(board);
    REQUIRE(board.side_to_move == WHITE);
    REQUIRE(board.fullmove_number == 1);
    REQUIRE(board.en_passant_file == 8);
    REQUIRE(board.move_stack.empty());

    reset_board(board);
    REQUIRE(board.occupied == 0);
}

TEST_CASE("Pawn moves")
{
    BoardState board;
    reset_board(board);

    uint8_t e2 = 12;
    uint8_t d7 = 51;

    place_piece(board, e2, make_piece(PAWN, WHITE));
    place_piece(board, d7, make_piece(PAWN, BLACK));

    // White 1-step
    Move m1 = make_move(e2, e2 + 8);
    make_move(board, m1);
    REQUIRE(piece_at(board, e2 + 8) == make_piece(PAWN, WHITE));
    REQUIRE(piece_at(board, e2) == make_piece(NONE, WHITE));
    unmake_move(board, m1);
    REQUIRE(piece_at(board, e2) == make_piece(PAWN, WHITE));

    // White 2-step
    Move m2 = make_move(e2, e2 + 16);
    make_move(board, m2);
    REQUIRE(board.en_passant_file == 4);
    unmake_move(board, m2);
    REQUIRE(board.en_passant_file == 8);

    // Black 1-step
    Move m3 = make_move(d7, d7 - 8);
    make_move(board, m3);
    REQUIRE(piece_at(board, d7 - 8) == make_piece(PAWN, BLACK));
    unmake_move(board, m3);
    REQUIRE(piece_at(board, d7) == make_piece(PAWN, BLACK));

    // Black 2-step
    Move m4 = make_move(d7, d7 - 16);
    make_move(board, m4);
    REQUIRE(board.en_passant_file == 3);
    unmake_move(board, m4);
    REQUIRE(board.en_passant_file == 8);

    // Pawn capture
    reset_board(board);
    place_piece(board, e4, make_piece(PAWN, WHITE));
    place_piece(board, d5, make_piece(PAWN, BLACK));

    Move cap = make_move(e4, d5);
    make_move(board, cap);
    REQUIRE(piece_at(board, d5) == make_piece(PAWN, WHITE));
    REQUIRE(piece_at(board, e4) == make_piece(NONE, WHITE));
    unmake_move(board, cap);
    REQUIRE(piece_at(board, e4) == make_piece(PAWN, WHITE));
    REQUIRE(piece_at(board, d5) == make_piece(PAWN, BLACK));

    // En passant
    reset_board(board);
    place_piece(board, e5, make_piece(PAWN, WHITE));
    place_piece(board, d7, make_piece(PAWN, BLACK));

    Move black_double = make_move(d7, d5);
    make_move(board, black_double);
    REQUIRE(board.en_passant_file == 3);

    Move ep = make_move(e5, d6);
    make_move(board, ep);
    REQUIRE(piece_at(board, d5) == make_piece(NONE, WHITE)); // captured
    unmake_move(board, ep);
    REQUIRE(piece_at(board, d5) == make_piece(PAWN, BLACK));
}

TEST_CASE("Knight moves")
{
    BoardState board;
    reset_board(board);
    place_piece(board, 28, make_piece(KNIGHT, WHITE)); // e4

    std::vector<Move> moves;
    generate_pseudo_legal_moves(board, moves);
    REQUIRE(moves.size() == 8);

    // Move and unmake
    Move m = moves[0];
    make_move(board, m);
    unmake_move(board, m);
    REQUIRE(piece_at(board, 28) == make_piece(KNIGHT, WHITE));
}

TEST_CASE("King moves and castling")
{
    BoardState board;
    reset_board(board);

    place_piece(board, 4, make_piece(KING, WHITE));
    place_piece(board, 0, make_piece(ROOK, WHITE));
    place_piece(board, 7, make_piece(ROOK, WHITE));

    board.castling_rights = CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN;

    // King-side castle
    Move k_castle = make_move(4, 6);
    make_move(board, k_castle);
    REQUIRE(piece_at(board, 6) == make_piece(KING, WHITE));
    REQUIRE(piece_at(board, 5) == make_piece(ROOK, WHITE));
    unmake_move(board, k_castle);
    REQUIRE(piece_at(board, 4) == make_piece(KING, WHITE));
    REQUIRE(piece_at(board, 7) == make_piece(ROOK, WHITE));

    // Queen-side castle
    Move q_castle = make_move(4, 2);
    make_move(board, q_castle);
    REQUIRE(piece_at(board, 2) == make_piece(KING, WHITE));
    REQUIRE(piece_at(board, 3) == make_piece(ROOK, WHITE));
    unmake_move(board, q_castle);
    REQUIRE(piece_at(board, 4) == make_piece(KING, WHITE));
    REQUIRE(piece_at(board, 0) == make_piece(ROOK, WHITE));
}

TEST_CASE("Promotions")
{
    BoardState board;
    reset_board(board);

    place_piece(board, 48, make_piece(PAWN, WHITE)); // a7

    for (uint8_t promo = 0; promo <= 3; ++promo)
    {
        Move m = make_promotion(48, 56, promo);
        make_move(board, m);
        REQUIRE(piece_type(piece_at(board, 56)) == static_cast<PieceType>(promo));
        unmake_move(board, m);
        REQUIRE(piece_at(board, 48) == make_piece(PAWN, WHITE));
        REQUIRE(piece_at(board, 56) == make_piece(NONE, WHITE));
    }
}

TEST_CASE("Check and checkmate detection")
{
    BoardState board;
    reset_board(board);

    // Simple check
    place_piece(board, 4, make_piece(KING, WHITE));
    place_piece(board, 12, make_piece(ROOK, BLACK));

    REQUIRE(is_in_check(board));

    // Checkmate
    reset_board(board);
    board.side_to_move = WHITE;
    place_piece(board, 63, make_piece(KING, WHITE));
    place_piece(board, 54, make_piece(ROOK, BLACK));
    place_piece(board, 55, make_piece(ROOK, BLACK));
    place_piece(board, 0, make_piece(KING, BLACK));

    REQUIRE(is_checkmate(board));
    REQUIRE(check_game_result(board) == BLACK_WINS);

    // Stalemate
    reset_board(board);
    board.side_to_move = WHITE;
    place_piece(board, 0, make_piece(KING, WHITE));
    place_piece(board, 10, make_piece(QUEEN, BLACK));

    REQUIRE(is_stalemate(board));
    REQUIRE(check_game_result(board) == DRAW_STALEMATE);
}

TEST_CASE("Insufficient material draws")
{
    BoardState board;
    reset_board(board);

    // Only kings
    place_piece(board, 0, make_piece(KING, WHITE));
    place_piece(board, 63, make_piece(KING, BLACK));
    REQUIRE(is_draw_by_insufficient_material(board));

    // King + knight vs king
    reset_board(board);
    place_piece(board, 0, make_piece(KING, WHITE));
    place_piece(board, 1, make_piece(KNIGHT, WHITE));
    place_piece(board, 63, make_piece(KING, BLACK));
    REQUIRE(is_draw_by_insufficient_material(board));

    // King + bishop vs king + bishop (same color)
    reset_board(board);
    place_piece(board, 0, make_piece(KING, WHITE));
    place_piece(board, 2, make_piece(BISHOP, WHITE));
    place_piece(board, 63, make_piece(KING, BLACK));
    place_piece(board, 61, make_piece(BISHOP, BLACK));
    REQUIRE(is_draw_by_insufficient_material(board));
}

TEST_CASE("Attack maps")
{
    BoardState board;
    reset_board(board);

    place_piece(board, 28, make_piece(KNIGHT, WHITE)); // e4
    REQUIRE(get_knight_attacks(28) & square_bb(11));
    REQUIRE(get_knight_attacks(28) & square_bb(13));
    REQUIRE(get_knight_attacks(28) & square_bb(18));
}

TEST_CASE("Move unmake complex scenario")
{
    BoardState board;
    reset_board(board);

    // Setup pawns
    place_piece(board, 12, make_piece(PAWN, WHITE)); // e2
    place_piece(board, 51, make_piece(PAWN, BLACK)); // d7

    // e2 -> e4
    Move w1 = make_move(12, 28);
    make_move(board, w1);

    // e4 -> e5

    Move w1b = make_move(28, 36);
    make_move(board, w1b);

    // d7 -> d5

    Move b1 = make_move(51, 35);
    make_move(board, b1);
    REQUIRE(board.en_passant_file == 3);

    // e5 x d6 en passant

    Move ep = make_move(36, 43);
    make_move(board, ep);

    REQUIRE(piece_at(board, 43) == make_piece(PAWN, WHITE));
    REQUIRE(piece_at(board, 35) == make_piece(NONE, WHITE));

    // Undo EP
    unmake_move(board, ep);
    REQUIRE(piece_at(board, 36) == make_piece(PAWN, WHITE));
    REQUIRE(piece_at(board, 35) == make_piece(PAWN, BLACK));

    // Undo d7-d5
    unmake_move(board, b1);
    REQUIRE(piece_at(board, 51) == make_piece(PAWN, BLACK));

    // Undo e4-e5
    unmake_move(board, w1b);
    REQUIRE(piece_at(board, 28) == make_piece(PAWN, WHITE));

    // Undo e2-e4
    unmake_move(board, w1);
    REQUIRE(piece_at(board, 12) == make_piece(PAWN, WHITE));
}
