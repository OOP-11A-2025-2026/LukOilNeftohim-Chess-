#ifndef CHESS_ENGINE_SEARCH_HPP
#define CHESS_ENGINE_SEARCH_HPP

#include "../core/board.hpp"
#include "../core/move.hpp"
#include <cstdint>
#include <vector>
#include <chrono>

namespace chess {

struct SearchResult {
    Move best_move;
    int32_t score;
    uint32_t depth;
    uint64_t nodes_searched;
    std::chrono::milliseconds time_elapsed;
    std::vector<Move> principal_variation;
};

struct SearchLimits {
    uint32_t max_depth = 64;
    uint64_t max_nodes = UINT64_MAX;
    std::chrono::milliseconds max_time = std::chrono::hours(1);
    bool infinite = false;
};

struct TTEntry {
    uint64_t hash;
    Move best_move;
    int32_t score;
    uint8_t depth;
    uint8_t flags;  // exact, lower bound, upper bound
};

struct SearchContext {
    std::vector<TTEntry> transposition_table;
    std::array<std::array<int32_t, 64>, 64> history_table;
    uint64_t nodes;
    std::chrono::steady_clock::time_point start_time;
    SearchLimits limits;
    
    SearchContext();
    void clear();
};

SearchResult search(const BoardState& board, const SearchLimits& limits);
int32_t alpha_beta(BoardState& board, SearchContext& ctx, int32_t alpha, int32_t beta, uint32_t depth);
int32_t quiescence_search(BoardState& board, SearchContext& ctx, int32_t alpha, int32_t beta);

void order_moves(const BoardState& board, std::vector<Move>& moves, Move tt_move);
int32_t move_score(const BoardState& board, Move move);

bool should_stop_search(const SearchContext& ctx);

} // namespace chess

#endif
