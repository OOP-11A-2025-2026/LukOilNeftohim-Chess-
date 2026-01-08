#ifndef CHESS_PARSER_FEN_HPP
#define CHESS_PARSER_FEN_HPP

#include "../core/board.hpp"
#include <string>
#include <optional>

namespace chess {

constexpr const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

std::optional<BoardState> parse_fen(const std::string& fen);
std::string board_to_fen(const BoardState& board);

bool is_valid_fen(const std::string& fen);

} // namespace chess

#endif
