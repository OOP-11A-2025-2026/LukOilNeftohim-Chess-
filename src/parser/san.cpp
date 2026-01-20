#include "chess/parser/san.hpp"

namespace chess {

static char piece_letter(PieceType pt) {
    switch (pt) {
        case KNIGHT: return 'N';
        case BISHOP: return 'B';
        case ROOK:   return 'R';
        case QUEEN:  return 'Q';
        case KING:   return 'K';
        default:     return '\0';
    }
}

static PieceType letter_to_piece(char c) {
    switch (std::toupper(c)) {
        case 'N': return KNIGHT;
        case 'B': return BISHOP;
        case 'R': return ROOK;
        case 'Q': return QUEEN;
        case 'K': return KING;
        default:  return PAWN;
    }
}

static bool is_file(char c) { return c >= 'a' && c <= 'h'; }
static bool is_rank(char c) { return c >= '1' && c <= '8'; }

static uint8_t sq_from_file_rank(char f, char r) {
    return (r - '1') * 8 + (f - 'a');
}

static bool gives_check(BoardState& board, Move m) {
    make_move(board, m);
    bool check = is_in_check(board);
    unmake_move(board, m);
    return check;
}

static bool gives_mate(BoardState& board, Move m) {
    make_move(board, m);
    if (!is_in_check(board)) {
        unmake_move(board, m);
        return false;
    }

    // if no legal reply exists -> mate
    for (uint8_t from = 0; from < 64; ++from) {
        uint8_t p = piece_at(board, from);
        if (p == NONE || piece_color(p) != board.side_to_move) continue;

        Bitboard attacks = 0;
        PieceType pt = piece_type(p);
        if (pt == PAWN) attacks = get_pawn_attacks(from, board.side_to_move);
        else if (pt == KNIGHT) attacks = get_knight_attacks(from);
        else if (pt == BISHOP) attacks = get_bishop_attacks(from, board.occupied);
        else if (pt == ROOK) attacks = get_rook_attacks(from, board.occupied);
        else if (pt == QUEEN) attacks = get_queen_attacks(from, board.occupied);
        else if (pt == KING) attacks = get_king_attacks(from);

        while (attacks) {
            uint8_t to = pop_lsb(attacks);
            Move reply = make_move(from, to);
            make_move(board, reply);
            bool ok = !is_in_check(board);
            unmake_move(board, reply);
            if (ok) {
                unmake_move(board, m);
                return false;
            }
        }
    }

    unmake_move(board, m);
    return true;
}

std::optional<Move> parse_san(const BoardState& board, const std::string& san) {
    std::string s = san;

    // Strip check/mate
    if (!s.empty() && (s.back() == '+' || s.back() == '#'))
        s.pop_back();

    // Castling
    if (s == "O-O" || s == "0-0") {
        uint8_t e = board.side_to_move == WHITE ? 4 : 60;
        return make_move(e, e + 2);
    }
    if (s == "O-O-O" || s == "0-0-0") {
        uint8_t e = board.side_to_move == WHITE ? 4 : 60;
        return make_move(e, e - 2);
    }

    size_t idx = 0;
    PieceType pt = PAWN;

    if (std::isupper(s[idx]) && s[idx] != 'O') {
        pt = letter_to_piece(s[idx]);
        idx++;
    }

    char dis_file = 0, dis_rank = 0;
    if (idx < s.size() && is_file(s[idx])) {
        dis_file = s[idx++];
    }
    if (idx < s.size() && is_rank(s[idx])) {
        dis_rank = s[idx++];
    }

    if (idx < s.size() && s[idx] == 'x') idx++;

    if (idx + 1 >= s.size()) return std::nullopt;
    uint8_t to = sq_from_file_rank(s[idx], s[idx + 1]);
    idx += 2;

    PieceType promo = NONE;
    if (idx < s.size() && s[idx] == '=') {
        promo = letter_to_piece(s[idx + 1]);
    }

    for (uint8_t from = 0; from < 64; ++from) {
        uint8_t p = piece_at(board, from);
        if (p == NONE) continue;
        if (piece_color(p) != board.side_to_move) continue;
        if (piece_type(p) != pt) continue;

        if (dis_file && from % 8 != dis_file - 'a') continue;
        if (dis_rank && from / 8 != dis_rank - '1') continue;

        Bitboard attacks = 0;
        if (pt == PAWN) attacks = get_pawn_attacks(from, board.side_to_move);
        else if (pt == KNIGHT) attacks = get_knight_attacks(from);
        else if (pt == BISHOP) attacks = get_bishop_attacks(from, board.occupied);
        else if (pt == ROOK) attacks = get_rook_attacks(from, board.occupied);
        else if (pt == QUEEN) attacks = get_queen_attacks(from, board.occupied);
        else if (pt == KING) attacks = get_king_attacks(from);

        if (!(attacks & square_bb(to))) continue;

        Move m = (promo != NONE)
            ? make_promotion(from, to, promo - KNIGHT)
            : make_move(from, to);

        BoardState tmp = board;
        make_move(tmp, m);
        if (!is_in_check(tmp))
            return m;
    }

    return std::nullopt;
}

std::string move_to_san(const BoardState& board, Move move) {
    uint8_t from = move_from(move);
    uint8_t to   = move_to(move);

    uint8_t piece = piece_at(board, from);
    PieceType pt  = piece_type(piece);

    // Castling
    if (pt == KING && std::abs((int)to - (int)from) == 2) {
        std::string s = (to > from) ? "O-O" : "O-O-O";
        BoardState tmp = board;
        if (gives_mate(tmp, move)) s += "#";
        else if (gives_check(tmp, move)) s += "+";
        return s;
    }

    std::string san;

    if (pt != PAWN)
        san += piece_letter(pt);

    // Disambiguation
    if (pt != PAWN && pt != KING) {
        bool same_file = false, same_rank = false;
        for (uint8_t sq = 0; sq < 64; ++sq) {
            if (sq == from) continue;
            uint8_t p = piece_at(board, sq);
            if (p == NONE) continue;
            if (piece_type(p) == pt && piece_color(p) == piece_color(piece)) {
                Bitboard attacks = 0;
                if (pt == KNIGHT) attacks = get_knight_attacks(sq);
                else if (pt == BISHOP) attacks = get_bishop_attacks(sq, board.occupied);
                else if (pt == ROOK) attacks = get_rook_attacks(sq, board.occupied);
                else if (pt == QUEEN) attacks = get_queen_attacks(sq, board.occupied);
                if (attacks & square_bb(to)) {
                    if (sq % 8 == from % 8) same_file = true;
                    if (sq / 8 == from / 8) same_rank = true;
                }
            }
        }
        if (same_file) san += char('1' + from / 8);
        else if (same_rank) san += char('a' + from % 8);
    }

    // Capture
    if (piece_at(board, to) != NONE || (pt == PAWN && from % 8 != to % 8)) {
        if (pt == PAWN)
            san += char('a' + from % 8);
        san += 'x';
    }

    san += char('a' + to % 8);
    san += char('1' + to / 8);

    // Promotion
    if (pt == PAWN && (to / 8 == 0 || to / 8 == 7)) {
        san += '=';
        san += piece_letter(static_cast<PieceType>(move_promotion(move) + KNIGHT));
    }

    BoardState tmp = board;
    if (gives_mate(tmp, move)) san += "#";
    else if (gives_check(tmp, move)) san += "+";

    return san;
}

} // namespace chess
