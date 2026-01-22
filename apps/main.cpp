#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cstdint>
#include <algorithm>

using namespace std;

// --- TYPES & CONSTANTS ---
typedef uint64_t Bitboard;
enum Color
{
    WHITE,
    BLACK,
    BOTH
};
enum PieceType
{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
};

// Move encoding: From (6 bits), To (6 bits), Promotion (4 bits)
typedef uint16_t Move;
#define MOVE_NONE 0
inline Move create_move(int f, int t) { return (f & 0x3F) | ((t & 0x3F) << 6); }
inline int get_from(Move m) { return m & 0x3F; }
inline int get_to(Move m) { return (m >> 6) & 0x3F; }

// --- BOARD STRUCTURE ---
struct BoardState
{
    Bitboard pieces_bb[6] = {0};
    Bitboard colors_bb[2] = {0};
    Bitboard occupied = 0;
    Color side_to_move = WHITE;

    // Simple Square to Piece lookup
    PieceType get_piece_at(int sq)
    {
        Bitboard mask = 1ULL << sq;
        for (int i = 0; i < 6; i++)
        {
            if (pieces_bb[i] & mask)
                return (PieceType)i;
        }
        return NONE;
    }
};

// --- CORE LOGIC ---
void place_piece(BoardState &b, int sq, PieceType type, Color c)
{
    Bitboard mask = 1ULL << sq;
    b.pieces_bb[type] |= mask;
    b.colors_bb[c] |= mask;
    b.occupied |= mask;
}

void init_board(BoardState &b)
{
    // Pawns
    for (int i = 0; i < 8; i++)
    {
        place_piece(b, 8 + i, PAWN, WHITE);
        place_piece(b, 48 + i, PAWN, BLACK);
    }
    // Pieces (Simplified indices for demonstration)
    PieceType back_rank[] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};
    for (int i = 0; i < 8; i++)
    {
        place_piece(b, i, back_rank[i], WHITE);
        place_piece(b, 56 + i, back_rank[i], BLACK);
    }
}

// Simple Move Execution
void make_move(BoardState &b, Move m)
{
    int from = get_from(m);
    int to = get_to(m);
    PieceType p = b.get_piece_at(from);
    Color c = (b.colors_bb[WHITE] & (1ULL << from)) ? WHITE : BLACK;

    // Clear from
    b.pieces_bb[p] &= ~(1ULL << from);
    b.colors_bb[c] &= ~(1ULL << from);

    // Clear destination (capture)
    PieceType captured = b.get_piece_at(to);
    if (captured != NONE)
    {
        b.pieces_bb[captured] &= ~(1ULL << to);
        b.colors_bb[1 - c] &= ~(1ULL << to);
    }

    // Place on to
    b.pieces_bb[p] |= (1ULL << to);
    b.colors_bb[c] |= (1ULL << to);
    b.occupied = b.colors_bb[WHITE] | b.colors_bb[BLACK];
    b.side_to_move = (b.side_to_move == WHITE) ? BLACK : WHITE;
}

// --- UTILS & UI ---
void render_board(BoardState &b)
{
    const char *symbols[] = {"P", "N", "B", "R", "Q", "K"};
    const char *symbols_black[] = {"p", "n", "b", "r", "q", "k"};

    cout << "\n  a b c d e f g h\n";
    for (int r = 7; r >= 0; r--)
    {
        cout << r + 1 << " ";
        for (int f = 0; f < 8; f++)
        {
            int sq = r * 8 + f;
            PieceType p = b.get_piece_at(sq);
            if (p == NONE)
                cout << ". ";
            else
            {
                if (b.colors_bb[WHITE] & (1ULL << sq))
                    cout << symbols[p] << " ";
                else
                    cout << symbols_black[p] << " ";
            }
        }
        cout << r + 1 << "\n";
    }
    cout << "  a b c d e f g h\n\n";
}

Move parse_input(string input)
{
    if (input.length() < 4)
        return MOVE_NONE;
    int f = (input[0] - 'a') + (input[1] - '1') * 8;
    int t = (input[2] - 'a') + (input[3] - '1') * 8;
    if (f < 0 || f > 63 || t < 0 || t > 63)
        return MOVE_NONE;
    return create_move(f, t);
}

// --- MAIN LOOP ---
int main()
{
    BoardState board;
    init_board(board);
    string input;

    cout << "Chess Engine CLI (Enter moves like 'e2e4' or 'quit')\n";

    while (true)
    {
        render_board(board);
        cout << (board.side_to_move == WHITE ? "White" : "Black") << " to move: ";
        cin >> input;

        if (input == "quit")
            break;

        Move m = parse_input(input);
        if (m == MOVE_NONE)
        {
            cout << "Invalid format! Use 'e2e4'.\n";
            continue;
        }

        // Basic check if a piece exists at 'from'
        if (board.get_piece_at(get_from(m)) == NONE)
        {
            cout << "No piece at starting square!\n";
            continue;
        }

        make_move(board, m);
    }

    return 0;
}