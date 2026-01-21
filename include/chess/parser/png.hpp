#ifndef CHESS_PARSER_PNG_HPP
#define CHESS_PARSER_PNG_HPP

#include "../core/board.hpp"
#include "../core/move.hpp"
#include <string>
#include <vector>
#include <optional>

namespace chess {

struct PGNGame {
    std::string event;
    std::string site;
    std::string date;
    std::string white;
    std::string black;
    std::string result;
    
    BoardState starting_position;
    std::vector<Move> moves;
    std::vector<std::string> comments;
};

std::vector<PGNGame> parse_pgn_file(const std::string& content);
std::optional<PGNGame> parse_single_pgn(const std::string& content);
std::vector<PGNGame> parse_pgn_from_file(const std::string& filename);

std::string game_to_pgn(const PGNGame& game);
std::string moves_to_pgn(const BoardState& initial_board, const std::vector<Move>& moves);
void save_pgn_to_file(const std::string& filename, const PGNGame& game);
void save_pgn_games_to_file(const std::string& filename, const std::vector<PGNGame>& games);

} // namespace chess

#endif
