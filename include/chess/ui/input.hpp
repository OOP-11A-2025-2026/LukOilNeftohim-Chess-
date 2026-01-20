#ifndef CHESS_UI_INPUT_HPP
#define CHESS_UI_INPUT_HPP

#include <string>
#include <optional>

namespace chess {

enum class InputCommand {
    MOVE,           // Player entered a move
    HELP,           // Player wants help
    QUIT,           // Player wants to quit
    RESIGN,         // Player wants to resign
    DRAW_OFFER,     // Player offers a draw
    FLIP_BOARD,     // Player wants to flip board perspective
    INVALID         // Invalid input
};

struct UserInput {
    InputCommand command;
    std::string raw_input;
    
    // For move commands
    std::string move_notation;
};

class InputHandler {
public:
    // Get user input with prompt
    UserInput get_input(const std::string& prompt);
    
    // Validate move notation format
    bool is_valid_move_format(const std::string& notation);
    
    // Parse input string to determine command
    InputCommand parse_command(const std::string& input);
    
    // Trim whitespace from string
    static std::string trim(const std::string& str);
    
private:
    // Check if string matches algebraic notation patterns
    bool matches_algebraic_notation(const std::string& str);
};

// Display help information
void show_help();

// Display game commands
void show_commands();

} // namespace chess

#endif
