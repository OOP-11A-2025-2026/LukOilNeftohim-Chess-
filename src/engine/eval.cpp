#include "chess/engine/eval.hpp"

namespace chess {

const std::array<std::array<int32_t, 64>, 6> PST_MG = {};
const std::array<std::array<int32_t, 64>, 6> PST_EG = {};

int32_t evaluate(const BoardState& board) {
    // TODO: Implement evaluation
    return 0;
}

int32_t evaluate_material(const BoardState& board) {
    // TODO: Implement material evaluation
    return 0;
}

int32_t evaluate_position(const BoardState& board) {
    // TODO: Implement positional evaluation
    return 0;
}

int32_t evaluate_mobility(const BoardState& board) {
    // TODO: Implement mobility evaluation
    return 0;
}

int32_t evaluate_king_safety(const BoardState& board) {
    // TODO: Implement king safety evaluation
    return 0;
}

int32_t evaluate_pawn_structure(const BoardState& board) {
    // TODO: Implement pawn structure evaluation
    return 0;
}

} // namespace chess
