#include "chess/engine/search.hpp"

namespace chess {

SearchContext::SearchContext() {
    clear();
}

void SearchContext::clear() {
    transposition_table.clear();
    history_table = {};
    nodes = 0;
    start_time = std::chrono::steady_clock::now();
}

SearchResult search(const BoardState& board, const SearchLimits& limits) {
    // TODO: Implement search
    SearchResult result = {};
    return result;
}

int32_t alpha_beta(BoardState& board, SearchContext& ctx, int32_t alpha, int32_t beta, uint32_t depth) {
    // TODO: Implement alpha-beta search
    return 0;
}

int32_t quiescence_search(BoardState& board, SearchContext& ctx, int32_t alpha, int32_t beta) {
    // TODO: Implement quiescence search
    return 0;
}

void order_moves(const BoardState& board, std::vector<Move>& moves, Move tt_move) {
    // TODO: Implement move ordering
}

int32_t move_score(const BoardState& board, Move move) {
    // TODO: Implement move scoring
    return 0;
}

bool should_stop_search(const SearchContext& ctx) {
    // TODO: Implement search stopping condition
    return false;
}

} // namespace chess
