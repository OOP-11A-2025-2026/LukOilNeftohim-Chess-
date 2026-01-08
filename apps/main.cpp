#include "chess/core/board.hpp"
#include "chess/core/rules.hpp"
#include "chess/engine/search.hpp"
#include "chess/parser/fen.hpp"
#include "chess/parser/san.hpp"
#include "chess/ui/render.hpp"
#include <iostream>
#include <string>

using namespace chess;

int main(int argc, char* argv[]) {
    // Initialize board
    auto board = parse_fen(STARTING_FEN);
    if (!board) {
        std::cerr << "Failed to initialize board\n";
        return 1;
    }
    
    RenderOptions render_opts;
    render_opts.use_unicode = true;
    render_opts.show_coordinates = true;
    
    // Game loop
    while (true) {
        render_board(*board, render_opts);
        
        auto result = check_game_result(*board);
        if (result != IN_PROGRESS) {
            // Handle game end
            break;
        }
        
        std::cout << "\nEnter move (SAN format, 'quit' to exit): ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "quit") break;
        
        auto move = parse_san(*board, input);
        if (!move || !is_legal_move(*board, *move)) {
            std::cout << "Illegal move!\n";
            continue;
        }
        
        make_move(*board, *move);
    }
    
    return 0;
}
