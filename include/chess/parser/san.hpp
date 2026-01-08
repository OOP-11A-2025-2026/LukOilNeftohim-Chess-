#ifndef CHESS_PARSER_SAN_HPP
#define CHESS_PARSER_SAN_HPP

#include "../core/board.hpp"
#include "../core/move.hpp"
#include <string>
#include <optional>

namespace chess {

std::optional<Move> parse_san(const BoardState& board, const std::string& san);
std::string move_to_san(const BoardState& board, Move move);

std::string move_to_lan(Move move);
std::optional<Move> parse_lan(const std::string& lan);

std::string move_to_uci(Move move);
std::optional<Move> parse_uci(const std::string& uci);

} // namespace chess

#endif
