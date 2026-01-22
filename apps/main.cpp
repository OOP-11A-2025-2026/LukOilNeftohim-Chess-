#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "chess/core/board.hpp"
#include "chess/core/move.hpp"
#include "chess/core/piece.hpp"
#include "chess/core/rules.hpp"

using namespace chess;

// Convert square index (0-63) to algebraic notation (a1-h8)
std::string square_to_notation(uint8_t sq)
{
    char file = 'a' + (sq % 8);
    char rank = '1' + (sq / 8);
    return std::string(1, file) + std::string(1, rank);
}

// Convert algebraic notation to square index
uint8_t notation_to_square(const std::string &notation)
{
    if (notation.length() != 2)
        return 255;
    uint8_t file = notation[0] - 'a';
    uint8_t rank = notation[1] - '1';
    if (file > 7 || rank > 7)
        return 255;
    return rank * 8 + file;
}

// Parse input like "e2e4" or "e2 e4"
Move parse_move_input(const std::string &input, const BoardState &board)
{
    std::string from_str, to_str;

    if (input.find(' ') != std::string::npos)
    {
        std::istringstream iss(input);
        iss >> from_str >> to_str;
    }
    else if (input.length() == 4)
    {
        from_str = input.substr(0, 2);
        to_str = input.substr(2, 2);
    }
    else
    {
        return MOVE_NONE;
    }

    uint8_t from = notation_to_square(from_str);
    uint8_t to = notation_to_square(to_str);

    if (from == 255 || to == 255)
        return MOVE_NONE;

    return make_move(from, to);
}

// Render board in standard orientation
void render_board(const BoardState &board)
{
    std::cout << "\n  a b c d e f g h\n";
    for (int r = 7; r >= 0; r--)
    {
        std::cout << (r + 1) << " ";
        for (int f = 0; f < 8; f++)
        {
            uint8_t sq = r * 8 + f;
            uint8_t piece = piece_at(board, sq);

            if (piece_type(piece) == NONE)
                std::cout << ". ";
            else
            {
                char c = piece_to_char(piece);
                std::cout << c << " ";
            }
        }
        std::cout << (r + 1) << "\n";
    }
    std::cout << "  a b c d e f g h\n\n";
}

int main()
{
    BoardState board;
    set_starting_position(board);
    std::string input;

    std::cout << "Chess Engine CLI\n";
    std::cout << "Enter moves like 'e2e4' or 'e2 e4' or 'quit' to exit\n";
    std::cout << "Type 'moves' to see all legal moves\n";
    std::cout << "Type 'fen' to see FEN position\n\n";

    while (true)
    {
        render_board(board);

        // Check game status
        GameResult result = check_game_result(board);
        if (result == WHITE_WINS)
        {
            std::cout << "Game Over: Black is Checkmate - White Wins!\n";
            break;
        }
        else if (result == BLACK_WINS)
        {
            std::cout << "Game Over: White is Checkmate - Black Wins!\n";
            break;
        }
        else if (result == DRAW_STALEMATE)
        {
            std::cout << "Game Over: Stalemate - Draw!\n";
            break;
        }

        if (is_checkmate(board))
        {
            std::cout << "Checkmate!\n";
            break;
        }
        else if (is_in_check(board))
        {
            std::cout << "Check!\n";
        }

        std::cout << (board.side_to_move == WHITE ? "White" : "Black") << " to move: ";
        std::getline(std::cin, input);

        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);

        if (input.empty())
            continue;

        if (input == "quit")
            break;

        if (input == "moves")
        {
            std::vector<Move> legal_moves;
            generate_legal_moves(board, legal_moves);
            std::cout << "Legal moves: ";
            for (const Move &m : legal_moves)
            {
                std::cout << square_to_notation(move_from(m)) << square_to_notation(move_to(m)) << " ";
            }
            std::cout << "\n";
            continue;
        }

        if (input == "fen")
        {
            std::cout << "FEN: (not implemented)\n";
            continue;
        }

        Move m = parse_move_input(input, board);
        if (m == MOVE_NONE)
        {
            std::cout << "Invalid format! Use 'e2e4' or 'e2 e4'.\n";
            continue;
        }

        // Check if move is legal
        std::vector<Move> legal_moves;
        generate_legal_moves(board, legal_moves);

        bool move_found = false;
        for (const Move &legal_move : legal_moves)
        {
            if (move_from(legal_move) == move_from(m) && move_to(legal_move) == move_to(m))
            {
                move_found = true;
                m = legal_move;
                break;
            }
        }

        if (!move_found)
        {
            std::cout << "Illegal move! Type 'moves' to see legal moves.\n";
            continue;
        }

        make_move(board, m);
    }

    return 0;
}