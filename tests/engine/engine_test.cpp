#include "chess/engine/uci.hpp"
#include "chess/core/board.hpp"
#include "chess/core/move.hpp"
#include "../catch2/catch_amalgamated.hpp"
#include <iostream>

TEST_CASE("UCIEngine basic functionality")
{
    chess::BoardState board;
    chess::UCIEngine engine;

    // NOTE: set this path to your Stockfish binary
    std::string stockfish_path = "/opt/homebrew/bin/stockfish";

    REQUIRE_NOTHROW(engine.start(stockfish_path));
    REQUIRE_NOTHROW(engine.new_game());

    SECTION("Get best move from starting position")
    {
        chess::Move best_move;
        REQUIRE_NOTHROW(best_move = engine.get_bestmove(board, 1));

        // The move should be a valid square-to-square move
        uint8_t from = chess::move_from(best_move);
        uint8_t to = chess::move_to(best_move);

        REQUIRE(from < 64);
        REQUIRE(to < 64);

        std::cout << "Best move from startpos: "
                  << static_cast<int>(from) << " -> "
                  << static_cast<int>(to) << std::endl;
    }

    SECTION("Make move and get next best move")
    {
        chess::Move first = engine.get_bestmove(board, 1);
        chess::place_piece(board, chess::move_to(first),
                           chess::piece_at(board, chess::move_from(first)));
        chess::remove_piece(board, chess::move_from(first));

        chess::Move second;
        REQUIRE_NOTHROW(second = engine.get_bestmove(board, 1));

        uint8_t from = chess::move_from(second);
        uint8_t to = chess::move_to(second);

        REQUIRE(from < 64);
        REQUIRE(to < 64);

        std::cout << "Next best move: "
                  << static_cast<int>(from) << " -> "
                  << static_cast<int>(to) << std::endl;
    }

    REQUIRE_NOTHROW(engine.stop());
}
