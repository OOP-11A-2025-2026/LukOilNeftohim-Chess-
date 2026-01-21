#include "chess/parser/png.hpp"
#include "chess/parser/san.hpp"
#include "chess/parser/fen.hpp"
#include "chess/storage/storage.hpp"
#include <sstream>
#include <cctype>
#include <algorithm>

namespace chess {

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Helper function to extract a tag value from PGN header
static std::string extract_tag_value(const std::string& line) {
    size_t start = line.find('"');
    size_t end = line.rfind('"');
    if (start != std::string::npos && end != std::string::npos && start < end) {
        return line.substr(start + 1, end - start - 1);
    }
    return "";
}

std::vector<PGNGame> parse_pgn_file(const std::string& content) {
    std::vector<PGNGame> games;
    std::istringstream stream(content);
    std::string line;
    std::string current_game_content;

    // Read the entire file and split by games
    while (std::getline(stream, line)) {
        if (line.empty() && !current_game_content.empty()) {
            // Empty line marks potential end of game
            auto game = parse_single_pgn(current_game_content);
            if (game) {
                games.push_back(game.value());
                current_game_content.clear();
            }
            continue;
        }
        current_game_content += line + "\n";
    }

    // Parse the last game if exists
    if (!current_game_content.empty()) {
        auto game = parse_single_pgn(current_game_content);
        if (game) {
            games.push_back(game.value());
        }
    }

    return games;
}

std::vector<PGNGame> parse_pgn_from_file(const std::string& filename) {
    std::string game_specs;
    std::vector<std::string> move_tokens = readFile(filename, game_specs);
    
    std::vector<PGNGame> games;
    PGNGame game;
    game.starting_position = BoardState();
    init_board(game.starting_position);
    
    // Parse header tags from game_specs
    std::istringstream header_stream(game_specs);
    std::string line;
    
    while (std::getline(header_stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] != '[') continue;
        
        size_t tag_end = line.find(' ');
        if (tag_end != std::string::npos) {
            std::string tag = line.substr(1, tag_end - 1);
            std::string value = extract_tag_value(line);
            
            if (tag == "Event") game.event = value;
            else if (tag == "Site") game.site = value;
            else if (tag == "Date") game.date = value;
            else if (tag == "White") game.white = value;
            else if (tag == "Black") game.black = value;
            else if (tag == "Result") game.result = value;
            else if (tag == "FEN") {
                auto fen_board = parse_fen(value);
                if (fen_board) {
                    game.starting_position = fen_board.value();
                }
            }
        }
    }
    
    // Parse moves from move_tokens
    for (const auto& token : move_tokens) {
        auto move = parse_san(game.starting_position, token);
        if (move) {
            game.moves.push_back(move.value());
            make_move(game.starting_position, move.value());
        }
    }
    
    if (!game.moves.empty() || !game.event.empty() || !game.white.empty()) {
        games.push_back(game);
    }
    
    return games;
}

void save_pgn_to_file(const std::string& filename, const PGNGame& game) {
    std::string pgn_content = game_to_pgn(game);
    writefile(filename, pgn_content);
}

void save_pgn_games_to_file(const std::string& filename, const std::vector<PGNGame>& games) {
    std::stringstream ss;
    
    for (size_t i = 0; i < games.size(); ++i) {
        ss << game_to_pgn(games[i]);
        if (i < games.size() - 1) {
            ss << "\n\n";  // Separate games with blank lines
        }
    }
    
    writefile(filename, ss.str());
}

std::optional<PGNGame> parse_single_pgn(const std::string& content) {
    PGNGame game;
    game.starting_position = BoardState();
    init_board(game.starting_position);

    std::istringstream stream(content);
    std::string line;
    bool in_header = true;
    std::string moves_section;

    // Parse header tags and moves
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty()) {
            in_header = false;
            continue;
        }

        if (in_header && line[0] == '[') {
            // Parse tag
            size_t tag_end = line.find(' ');
            if (tag_end != std::string::npos) {
                std::string tag = line.substr(1, tag_end - 1);
                std::string value = extract_tag_value(line);

                if (tag == "Event") game.event = value;
                else if (tag == "Site") game.site = value;
                else if (tag == "Date") game.date = value;
                else if (tag == "White") game.white = value;
                else if (tag == "Black") game.black = value;
                else if (tag == "Result") game.result = value;
                else if (tag == "FEN") {
                    auto fen_board = parse_fen(value);
                    if (fen_board) {
                        game.starting_position = fen_board.value();
                    }
                }
            }
        } else if (!in_header && !line.empty()) {
            moves_section += line + " ";
        }
    }

    // Parse moves from moves section
    if (!moves_section.empty()) {
        std::istringstream moves_stream(moves_section);
        std::string token;
        bool skip_next = false;

        while (moves_stream >> token) {
            // Skip move numbers (e.g., "1.", "2.")
            if (token.find('.') != std::string::npos) {
                continue;
            }

            // Skip result markers
            if (token == "1-0" || token == "0-1" || token == "1/2-1/2" || token == "*") {
                continue;
            }

            // Skip comments in braces
            if (token[0] == '{') {
                std::string comment;
                size_t close = token.find('}');
                if (close != std::string::npos) {
                    comment = token.substr(1, close - 1);
                } else {
                    // Read until closing brace
                    while (moves_stream >> token && token.find('}') == std::string::npos) {
                        comment += token + " ";
                    }
                    if (moves_stream >> token && token.find('}') != std::string::npos) {
                        comment += token.substr(0, token.find('}'));
                    }
                }
                if (!comment.empty()) {
                    game.comments.push_back(trim(comment));
                }
                continue;
            }

            // Parse move in SAN notation
            auto move = parse_san(game.starting_position, token);
            if (move) {
                game.moves.push_back(move.value());
                make_move(game.starting_position, move.value());
            }
        }
    }

    // Validate that we have at least some game data
    if (!game.moves.empty() || !game.event.empty() || !game.white.empty()) {
        return game;
    }

    return std::nullopt;
}

std::string game_to_pgn(const PGNGame& game) {
    std::stringstream ss;

    // Write header tags
    if (!game.event.empty()) ss << "[Event \"" << game.event << "\"]\n";
    if (!game.site.empty()) ss << "[Site \"" << game.site << "\"]\n";
    if (!game.date.empty()) ss << "[Date \"" << game.date << "\"]\n";
    if (!game.white.empty()) ss << "[White \"" << game.white << "\"]\n";
    if (!game.black.empty()) ss << "[Black \"" << game.black << "\"]\n";
    if (!game.result.empty()) ss << "[Result \"" << game.result << "\"]\n";

    ss << "\n";

    // Write moves
    BoardState board = game.starting_position;
    for (size_t i = 0; i < game.moves.size(); ++i) {
        // Add move number every two half-moves
        if (i % 2 == 0) {
            ss << (i / 2 + 1) << ". ";
        }

        std::string san = move_to_san(board, game.moves[i]);
        ss << san;

        if (i < game.moves.size() - 1) {
            ss << " ";
        }

        make_move(board, game.moves[i]);

        // Add comment if available
        if (i < game.comments.size() && !game.comments[i].empty()) {
            ss << " {" << game.comments[i] << "} ";
        }
    }

    // Add result
    if (!game.result.empty()) {
        ss << " " << game.result;
    }

    return ss.str();
}

std::string moves_to_pgn(const BoardState& initial_board, const std::vector<Move>& moves) {
    std::stringstream ss;
    BoardState board = initial_board;

    for (size_t i = 0; i < moves.size(); ++i) {
        // Add move number every two half-moves
        if (i % 2 == 0) {
            ss << (i / 2 + 1) << ". ";
        }

        std::string san = move_to_san(board, moves[i]);
        ss << san;

        if (i < moves.size() - 1) {
            ss << " ";
        }

        make_move(board, moves[i]);
    }

    return ss.str();
}

} // namespace chess
