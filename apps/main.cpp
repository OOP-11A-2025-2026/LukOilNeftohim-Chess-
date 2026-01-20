#include "chess/core/board.hpp"
#include "chess/core/rules.hpp"
#include "chess/engine/search.hpp"
#include "chess/parser/fen.hpp"
#include "chess/parser/san.hpp"
#include "chess/ui/render.hpp"
#include "chess/ui/input.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace chess;

// ANSI colors
constexpr const char *RESET = "\033[0m";
constexpr const char *BOLD = "\033[1m";
constexpr const char *GREEN = "\033[92m";
constexpr const char *RED = "\033[91m";
constexpr const char *YELLOW = "\033[93m";
constexpr const char *CYAN = "\033[96m";
constexpr const char *MAGENTA = "\033[95m";

int main(int argc, char *argv[])
{

    /*
        Linux - sudo apt install stockfish
        Mac - brew install stockfish
        Windows - Go to https://stockfishchess.org/download/

        chess::UCIEngine engine;
        engine.start("/usr/local/bin/stockfish"); // or wherever stockfish is
        engine.new_game();

        Move best = engine.get_bestmove(board, 10);
        make_move(board, best);
    */

    // Initialize board
    auto board = parse_fen(STARTING_FEN);
    if (!board)
    {
        std::cerr << RED << "Error: Failed to initialize board\n"
                  << RESET;
        return 1;
    }

    RenderOptions render_opts;
    render_opts.use_unicode = true;
    render_opts.show_coordinates = true;
    render_opts.flip_board = false;
    render_opts.highlight_last_move = true;
    render_opts.last_move = MOVE_NONE;

    int move_count = 0;
    bool pending_draw_offer = false;
    
    // Initialize input handler
    InputHandler input_handler;

    // Game loop
    while (true)
    {
        // Render the board
        render_board(*board, render_opts);

        // Show pending draw offer
        if (pending_draw_offer)
        {
            std::cout << YELLOW << "⚠ Draw offer is pending\n"
                      << RESET;
        }

        // Check game result
        auto result = check_game_result(*board);
        if (result != IN_PROGRESS)
        {
            std::cout << BOLD << GREEN << "Game Over! ";
            switch (result)
            {
            case WHITE_WINS:
                std::cout << "White Wins!\n";
                break;
            case BLACK_WINS:
                std::cout << "Black Wins!\n";
                break;
            case DRAW_STALEMATE:
                std::cout << "Draw by Stalemate\n";
                break;
            case DRAW_INSUFFICIENT:
                std::cout << "Draw by Insufficient Material\n";
                break;
            case DRAW_FIFTY_MOVE:
                std::cout << "Draw by 50-Move Rule\n";
                break;
            case DRAW_REPETITION:
                std::cout << "Draw by Repetition\n";
                break;
            default:
                std::cout << "Draw\n";
            }
            std::cout << RESET << "\n";
            break;
        }

        // Get user input
        std::string prompt = MAGENTA;
        prompt += (board->side_to_move == WHITE ? "White" : "Black");
        prompt += CYAN;
        prompt += " ▶ Enter move: ";
        prompt += RESET;
        
        UserInput user_input = input_handler.get_input(prompt);

        // Handle different input commands
        switch (user_input.command) {
            case InputCommand::QUIT:
                std::cout << YELLOW << "Thanks for playing!\n" << RESET;
                std::cout << CYAN << "Game ended after " << move_count << " moves.\n" << RESET;
                return 0;
            
            case InputCommand::HELP:
                show_help();
                continue;
            
            case InputCommand::FLIP_BOARD:
                render_opts.flip_board = !render_opts.flip_board;
                std::cout << YELLOW << (render_opts.flip_board ? "Board flipped (Black's perspective)\n" : "Board flipped (White's perspective)\n") << RESET;
                continue;
            
            case InputCommand::RESIGN: {
                std::string player = board->side_to_move == WHITE ? "White" : "Black";
                std::string opponent = board->side_to_move == WHITE ? "Black" : "White";
                std::cout << RED << player << " resigns.\n" << RESET;
                std::cout << GREEN << opponent << " wins!\n" << RESET;
                std::cout << CYAN << "Game ended after " << move_count << " moves.\n" << RESET;
                return 0;
            }
            
            case InputCommand::DRAW_OFFER: {
                if (pending_draw_offer) {
                    std::cout << YELLOW << "✓ Draw offer cancelled\n" << RESET;
                    pending_draw_offer = false;
                    continue;
                }
                
                std::string offering_player = board->side_to_move == WHITE ? "White" : "Black";
                std::string opponent = board->side_to_move == WHITE ? "Black" : "White";
                
                std::cout << CYAN << offering_player << " offers a draw.\n" << RESET;
                std::cout << MAGENTA << opponent << " ▶ Accept? (yes/no): " << RESET;
                
                std::string response;
                std::getline(std::cin, response);
                response = InputHandler::trim(response);
                
                if (response == "yes" || response == "y") {
                    std::cout << GREEN << "✓ Draw accepted. Game ends in a draw.\n" << RESET;
                    std::cout << CYAN << "Game ended after " << move_count << " moves.\n" << RESET;
                    return 0;
                } else if (response == "no" || response == "n") {
                    std::cout << RED << "✗ Draw offer rejected.\n" << RESET;
                    pending_draw_offer = false;
                    continue;
                } else {
                    std::cout << YELLOW << "Invalid response. Draw offer remains pending.\n" << RESET;
                    pending_draw_offer = true;
                    continue;
                }
            }
            
            case InputCommand::MOVE: {
                pending_draw_offer = false;
                
                // Try to parse move
                auto move = parse_san(*board, user_input.move_notation);
                
                if (!move) {
                    std::cout << RED << "✗ Invalid move format. Type 'help' for examples.\n" << RESET;
                    continue;
                }
                
                if (!is_legal_move(*board, *move)) {
                    std::cout << RED << "✗ Illegal move!\n" << RESET;
                    continue;
                }
                
                // Make the move
                render_opts.last_move = *move;
                make_move(*board, *move);
                move_count++;
                
                std::cout << GREEN << "✓ Move accepted\n" << RESET;
                break;
            }
            
            case InputCommand::INVALID:
                std::cout << RED << "✗ Invalid input. Type 'help' for command list.\n" << RESET;
                continue;
        }
    }

    std::cout << CYAN << "Game ended after " << move_count << " moves.\n"
              << RESET;
    return 0;
}
