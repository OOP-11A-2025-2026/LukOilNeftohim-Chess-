#include "chess/core/board.hpp"
#include "chess/core/rules.hpp"
#include "chess/engine/search.hpp"
#include "chess/parser/fen.hpp"
#include "chess/parser/san.hpp"
#include "chess/ui/render.hpp"
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
        std::cout << MAGENTA << (board->side_to_move == WHITE ? "White" : "Black");
        std::cout << CYAN << " ▶ Enter move (or 'help'/'quit'/'flip'/'draw'/'resign'): " << RESET;

        std::string input;
        std::getline(std::cin, input);

        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);

        if (input.empty())
            continue;

        // Handle special commands
        if (input == "quit" || input == "q")
        {
            std::cout << YELLOW << "Thanks for playing!\n"
                      << RESET;
            break;
        }

        if (input == "resign" || input == "r")
        {
            std::string player = board->side_to_move == WHITE ? "White" : "Black";
            std::string opponent = board->side_to_move == WHITE ? "Black" : "White";

            std::cout << RED << player << " wants to resign.\n"
                      << RESET;
            std::cout << GREEN << opponent << " wins!\n"
                      << RESET;
            break;
        }

        if (input == "draw" || input == "d")
        {
            if (pending_draw_offer)
            {
                // Same player offering draw again - cancel offer
                std::cout << YELLOW << "✓ Draw offer cancelled\n"
                          << RESET;
                pending_draw_offer = false;
                continue;
            }

            // Offer draw to opponent
            std::string offering_player = board->side_to_move == WHITE ? "White" : "Black";
            std::string opponent = board->side_to_move == WHITE ? "Black" : "White";

            std::cout << CYAN << offering_player << " offers a draw.\n"
                      << RESET;
            std::cout << MAGENTA << opponent << " ▶ Do you accept the draw? (yes/no): " << RESET;

            std::string response;
            std::getline(std::cin, response);

            // Trim response
            response.erase(0, response.find_first_not_of(" \t\n\r"));
            response.erase(response.find_last_not_of(" \t\n\r") + 1);

            if (response == "yes" || response == "y")
            {
                std::cout << GREEN << "✓ Draw accepted. Game ends in a draw.\n"
                          << RESET;
                break;
            }
            else if (response == "no" || response == "n")
            {
                std::cout << RED << "✗ Draw offer rejected.\n"
                          << RESET;
                pending_draw_offer = false;
                continue;
            }
            else
            {
                std::cout << YELLOW << "Invalid response. Draw offer remains pending.\n"
                          << RESET;
                pending_draw_offer = true;
                continue;
            }
        }

        if (input == "help" || input == "h")
        {
            std::cout << "\n"
                      << CYAN << "Move Format (SAN - Standard Algebraic Notation):\n"
                      << RESET;
            std::cout << "  " << GREEN << "e4" << RESET << "        - Pawn to e4\n";
            std::cout << "  " << GREEN << "Nf3" << RESET << "       - Knight to f3\n";
            std::cout << "  " << GREEN << "Bc4" << RESET << "       - Bishop to c4\n";
            std::cout << "  " << GREEN << "Qh5" << RESET << "       - Queen to h5\n";
            std::cout << "  " << GREEN << "Ra1" << RESET << "       - Rook to a1\n";
            std::cout << "  " << GREEN << "Kg1" << RESET << "       - King to g1\n";
            std::cout << "  " << GREEN << "exd5" << RESET << "      - Capture (pawn on e captures on d5)\n";
            std::cout << "  " << GREEN << "Nxe5" << RESET << "      - Knight captures on e5\n";
            std::cout << "  " << GREEN << "e8=Q" << RESET << "      - Promotion (pawn to Queen)\n";
            std::cout << "  " << GREEN << "O-O" << RESET << "       - Castling kingside\n";
            std::cout << "  " << GREEN << "O-O-O" << RESET << "     - Castling queenside\n\n";
            continue;
        }

        if (input == "flip")
        {
            render_opts.flip_board = !render_opts.flip_board;
            std::cout << YELLOW << (render_opts.flip_board ? "Board flipped (Black's perspective)\n" : "Board flipped (White's perspective)\n") << RESET;
            continue;
        }

        // Clear pending draw offer when making a move
        pending_draw_offer = false;

        // Try to parse move
        auto move = parse_san(*board, input);

        if (!move)
        {
            std::cout << RED << "✗ Invalid move format. Type 'help' for examples.\n"
                      << RESET;
            continue;
        }

        if (!is_legal_move(*board, *move))
        {
            std::cout << RED << "✗ Illegal move!\n"
                      << RESET;
            continue;
        }

        // Make the move
        render_opts.last_move = *move;
        make_move(*board, *move);
        move_count++;

        std::cout << GREEN << "✓ Move accepted\n"
                  << RESET;
    }

    std::cout << CYAN << "Game ended after " << move_count << " moves.\n"
              << RESET;
    return 0;
}
