#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include "chess/engine/uci.hpp"
#include "chess/core/board.hpp"

using namespace chess;

/*
  IMPORTANT:
  Change this path to where Stockfish is on YOUR machine
*/

// macOS (brew)
// /opt/homebrew/bin/stockfish
// Intel mac:
// /usr/local/bin/stockfish

// Linux:
// /usr/bin/stockfish

static const std::string STOCKFISH_PATH =
#ifdef __APPLE__
    "/opt/homebrew/bin/stockfish";
#elif defined(_WIN32)
    "stockfish.exe";
#else
    "/usr/bin/stockfish";
#endif

TEST_CASE("UCI engine starts and responds", "[uci]")
{
    UCIEngine engine;

    REQUIRE_NOTHROW(engine.start(STOCKFISH_PATH));
    REQUIRE_NOTHROW(engine.new_game());
    REQUIRE_NOTHROW(engine.stop());
}

TEST_CASE("UCI engine returns a legal move from start position", "[uci]")
{
    UCIEngine engine;
    engine.start(STOCKFISH_PATH);

    BoardState board;
    init_board(board);

    Move mv = engine.get_bestmove(board, 5);

    // A legal move must not be zero
    REQUIRE(mv != 0);

    engine.stop();
}

TEST_CASE("UCI engine can play multiple moves", "[uci]")
{
    UCIEngine engine;
    engine.start(STOCKFISH_PATH);

    BoardState board;
    init_board(board);

    Move m1 = engine.get_bestmove(board, 4);
    make_move(board, m1);
    REQUIRE(m1 != 0);

    Move m2 = engine.get_bestmove(board, 4);
    make_move(board, m2);
    REQUIRE(m2 != 0);

    engine.stop();
}

TEST_CASE("UCI engine survives repeated start/stop", "[uci]")
{
    for (int i = 0; i < 3; ++i)
    {
        UCIEngine engine;
        REQUIRE_NOTHROW(engine.start(STOCKFISH_PATH));
        REQUIRE_NOTHROW(engine.stop());
    }
}
