#include "chess/parser/fen.hpp"

namespace chess {

std::optional<BoardState> parse_fen(const std::string& fen) {
    if (!is_valid_fen(fen)) {
        return std::nullopt;
    }
    
    BoardState board = {};
    // TODO: Implement FEN parsing
    return board;
}

std::string board_to_fen(const BoardState& board) {
    // TODO: Implement FEN export
    return "";
}

bool is_valid_fen(const std::string& fen) {
    // TODO: Implement FEN validation
    return !fen.empty();
}

} // namespace chess
