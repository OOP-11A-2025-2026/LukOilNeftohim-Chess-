#include "chess/ui/render.hpp"
#include "chess/core/piece.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

namespace chess {

// ANSI color codes
constexpr const char* RESET = "\033[0m";
constexpr const char* BOLD = "\033[1m";
constexpr const char* LIGHT_SQ = "\033[47m";      // White background
constexpr const char* DARK_SQ = "\033[100m";      // Dark gray background
constexpr const char* WHITE_TEXT = "\033[97m";    // Bright white
constexpr const char* BLACK_TEXT = "\033[30m";    // Black text
constexpr const char* HIGHLIGHT = "\033[43m";    // Yellow background
constexpr const char* GREEN = "\033[92m";         // Green text
constexpr const char* RED = "\033[91m";           // Red text
constexpr const char* CYAN = "\033[96m";          // Cyan text
constexpr const char* YELLOW = "\033[93m";        // Yellow text

char get_piece_char(uint8_t piece) {
    if (piece == make_piece(NONE, WHITE)) return ' ';
    
    const char* pieces = "PNBRQK";
    PieceType type = piece_type(piece);
    char c = pieces[type];
    
    return piece_color(piece) == WHITE ? c : c + 32; // lowercase for black
}

std::string square_to_string(uint8_t square) {
    if (square > 63) return "??";
    int file = square % 8;
    int rank = square / 8;
    std::string result;
    result += char('a' + file);
    result += char('1' + rank);
    return result;
}

uint8_t string_to_square(const std::string& str) {
    if (str.length() != 2) return 64;
    int file = str[0] - 'a';
    int rank = str[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7) return 64;
    return rank * 8 + file;
}

void render_board(const BoardState& board, const RenderOptions& options) {
    // Clear screen
    std::cout << "\033[2J\033[H";
    
    int start_rank = options.flip_board ? 0 : 7;
    int end_rank = options.flip_board ? 8 : -1;
    int rank_step = options.flip_board ? 1 : -1;
    
    // Top file labels
    std::cout << "    a  b  c  d  e  f  g  h\n";
    
    // Board ranks
    for (int rank = start_rank; rank != end_rank; rank += rank_step) {
        std::cout << BOLD << " " << (rank + 1) << " " << RESET;
        
        for (int file = 0; file < 8; file++) {
            uint8_t square = rank * 8 + file;
            bool is_light = (file + rank) % 2 == 0;
            
            // Determine background
            const char* bg = is_light ? LIGHT_SQ : DARK_SQ;
            
            // Check if this is last move highlight
            bool is_highlight = false;
            if (options.highlight_last_move && options.last_move != MOVE_NONE) {
                uint8_t from = move_from(options.last_move);
                uint8_t to = move_to(options.last_move);
                if (square == from || square == to) {
                    bg = HIGHLIGHT;
                    is_highlight = true;
                }
            }
            
            // Get piece at square
            uint8_t piece = piece_at(board, square);
            
            // Display piece or empty square
            std::cout << bg;
            if (piece != make_piece(NONE, WHITE)) {
                if (piece_color(piece) == WHITE) {
                    std::cout << WHITE_TEXT << BOLD;
                } else {
                    std::cout << BLACK_TEXT << BOLD;
                }
                
                // Use unicode pieces
                PieceType type = piece_type(piece);
                std::cout << " " << UNICODE_PIECES[piece] << " ";
            } else {
                // Show empty square
                std::cout << "   ";
            }
            std::cout << RESET;
        }
        std::cout << BOLD << " " << (rank + 1) << RESET << "\n";
    }
    
    // Bottom file labels
    std::cout << "    a  b  c  d  e  f  g  h\n";
    
    // Game info
    std::cout << "\n" << CYAN << "   Status:" << RESET;
    
    if (board.side_to_move == WHITE) {
        std::cout << YELLOW << " White to move" << RESET;
    } else {
        std::cout << RED << " Black to move" << RESET;
    }
    
    std::cout << " │ Move: " << BOLD << board.fullmove_number << RESET << "\n";
    
    // Castling rights
    std::cout << CYAN << "   Castling:" << RESET << " ";
    if (board.castling_rights == 0) {
        std::cout << RED << "None" << RESET;
    } else {
        if (board.castling_rights & 0x1) std::cout << GREEN << "K" << RESET;
        if (board.castling_rights & 0x2) std::cout << GREEN << "Q" << RESET;
        if (board.castling_rights & 0x4) std::cout << GREEN << "k" << RESET;
        if (board.castling_rights & 0x8) std::cout << GREEN << "q" << RESET;
    }
    std::cout << "\n\n";
}

void render_board_simple(const BoardState& board) {
    std::cout << "\n  a b c d e f g h\n";
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; file++) {
            uint8_t square = rank * 8 + file;
            uint8_t piece = piece_at(board, square);
            
            if (piece != make_piece(NONE, WHITE)) {
                PieceType type = piece_type(piece);
                std::cout << UNICODE_PIECES[type] << " ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << rank + 1 << "\n";
    }
    std::cout << "  a b c d e f g h\n\n";
}

void render_move_list(const std::vector<Move>& moves, int columns) {
    if (moves.empty()) {
        std::cout << YELLOW << "No legal moves available" << RESET << "\n";
        return;
    }
    
    std::cout << CYAN << "Available moves (" << moves.size() << "): " << RESET;
    
    int count = 0;
    for (const auto& move : moves) {
        if (count > 0 && count % columns == 0) {
            std::cout << "\n                  ";
        }
        
        uint8_t from = move_from(move);
        uint8_t to = move_to(move);
        uint8_t promo = move_promotion(move);
        
        std::cout << square_to_string(from) << "-" << square_to_string(to);
        
        if (promo != 0) {
            std::cout << "=" << "NBRQ"[promo];
        }
        
        std::cout << "  ";
        count++;
    }
    std::cout << "\n\n";
}

} // namespace chess
