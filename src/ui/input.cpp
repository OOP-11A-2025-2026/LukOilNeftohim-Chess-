#include "chess/ui/input.hpp"
#include <iostream>
#include <algorithm>
#include <regex>

namespace chess {

// ANSI color codes
constexpr const char *RESET = "\033[0m";
constexpr const char *BOLD = "\033[1m";
constexpr const char *GREEN = "\033[92m";
constexpr const char *RED = "\033[91m";
constexpr const char *YELLOW = "\033[93m";
constexpr const char *CYAN = "\033[96m";
constexpr const char *MAGENTA = "\033[95m";

std::string InputHandler::trim(const std::string& str) {
    std::string result = str;
    // Trim leading whitespace
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    // Trim trailing whitespace
    result.erase(result.find_last_not_of(" \t\n\r") + 1);
    return result;
}

InputCommand InputHandler::parse_command(const std::string& input) {
    std::string lower_input = input;
    std::transform(lower_input.begin(), lower_input.end(), lower_input.begin(), ::tolower);
    
    if (lower_input == "quit" || lower_input == "q") {
        return InputCommand::QUIT;
    }
    if (lower_input == "resign" || lower_input == "r") {
        return InputCommand::RESIGN;
    }
    if (lower_input == "draw" || lower_input == "d") {
        return InputCommand::DRAW_OFFER;
    }
    if (lower_input == "help" || lower_input == "h" || lower_input == "?") {
        return InputCommand::HELP;
    }
    if (lower_input == "flip" || lower_input == "f") {
        return InputCommand::FLIP_BOARD;
    }
    
    // Check if it's a valid move notation
    if (is_valid_move_format(input)) {
        return InputCommand::MOVE;
    }
    
    return InputCommand::INVALID;
}

bool InputHandler::is_valid_move_format(const std::string& notation) {
    return matches_algebraic_notation(notation);
}

bool InputHandler::matches_algebraic_notation(const std::string& str) {
    // Regex patterns for chess move notation
    // Matches: e4, Nf3, exd5, e8=Q, O-O, O-O-O, etc.
    
    std::regex patterns[] = {
        std::regex("^[a-h][1-8]$"),                                    // e4 (pawn move)
        std::regex("^[NBRQK][a-h]?[1-8]?x?[a-h][1-8]$"),             // Nf3, Nxe5, etc.
        std::regex("^[a-h]x[a-h][1-8]$"),                             // exd5 (pawn capture)
        std::regex("^[a-h][1-8]=[NBRQ]$"),                            // e8=Q (promotion)
        std::regex("^[a-h]x[a-h][1-8]=[NBRQ]$"),                      // exd8=Q (capture + promotion)
        std::regex("^O-O$"),                                           // O-O (kingside castling)
        std::regex("^O-O-O$"),                                         // O-O-O (queenside castling)
        std::regex("^[NBRQK][a-h][1-8]$"),                            // Nc3, Bd2 (simple piece moves)
        std::regex("^[NBRQK][a-h]?[1-8]?[a-h][1-8]$"),              // Nac3, N1c3 (disambiguation)
    };
    
    for (const auto& pattern : patterns) {
        if (std::regex_match(str, pattern)) {
            return true;
        }
    }
    
    return false;
}

UserInput InputHandler::get_input(const std::string& prompt) {
    std::cout << prompt;
    std::cout.flush();
    
    std::string raw_input;
    std::getline(std::cin, raw_input);
    
    raw_input = trim(raw_input);
    
    if (raw_input.empty()) {
        return {InputCommand::INVALID, "", ""};
    }
    
    InputCommand cmd = parse_command(raw_input);
    
    return {cmd, raw_input, raw_input};
}

void show_help() {
    std::cout << "\n"
              << CYAN << BOLD << "═══════════════════════════════════════════════════════════\n"
              << "  CHESS MOVE NOTATION HELP\n"
              << "═══════════════════════════════════════════════════════════" << RESET << "\n\n";
    
    std::cout << CYAN << "Basic Moves (Standard Algebraic Notation):\n" << RESET;
    std::cout << "  " << GREEN << "e4" << RESET << "        - Pawn to e4\n";
    std::cout << "  " << GREEN << "Nf3" << RESET << "       - Knight to f3\n";
    std::cout << "  " << GREEN << "Bc4" << RESET << "       - Bishop to c4\n";
    std::cout << "  " << GREEN << "Qh5" << RESET << "       - Queen to h5\n";
    std::cout << "  " << GREEN << "Ra1" << RESET << "       - Rook to a1\n";
    std::cout << "  " << GREEN << "Kg1" << RESET << "       - King to g1\n\n";
    
    std::cout << CYAN << "Special Moves:\n" << RESET;
    std::cout << "  " << GREEN << "exd5" << RESET << "      - Capture (pawn on e captures on d5)\n";
    std::cout << "  " << GREEN << "Nxe5" << RESET << "      - Knight captures on e5\n";
    std::cout << "  " << GREEN << "e8=Q" << RESET << "      - Promotion (pawn to Queen)\n";
    std::cout << "  " << GREEN << "exd8=Q" << RESET << "    - Capture and promote\n";
    std::cout << "  " << GREEN << "O-O" << RESET << "       - Castling kingside\n";
    std::cout << "  " << GREEN << "O-O-O" << RESET << "     - Castling queenside\n\n";
    
    show_commands();
}

void show_commands() {
    std::cout << CYAN << BOLD << "Game Commands:\n" << RESET;
    std::cout << "  " << YELLOW << "help" << RESET << " or " << YELLOW << "h" << RESET << "      - Show this help\n";
    std::cout << "  " << YELLOW << "flip" << RESET << " or " << YELLOW << "f" << RESET << "      - Flip board perspective\n";
    std::cout << "  " << YELLOW << "draw" << RESET << " or " << YELLOW << "d" << RESET << "      - Offer or accept a draw\n";
    std::cout << "  " << YELLOW << "resign" << RESET << " or " << YELLOW << "r" << RESET << "    - Resign the game\n";
    std::cout << "  " << YELLOW << "quit" << RESET << " or " << YELLOW << "q" << RESET << "      - Quit without saving\n";
    std::cout << "\n";
}

} // namespace chess
