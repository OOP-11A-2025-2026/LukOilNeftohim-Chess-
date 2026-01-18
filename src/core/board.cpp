#include "chess/core/board.hpp"
#include <cstring>

namespace chess
{

    // Zobrist hashing constants (placeholder)
    const std::array<std::array<uint64_t, 64>, 12> ZOBRIST_PIECES = {};
    const std::array<uint64_t, 16> ZOBRIST_CASTLING = {};
    const std::array<uint64_t, 8> ZOBRIST_EN_PASSANT = {};
    const uint64_t ZOBRIST_SIDE = 0;

    BoardState::BoardState()
    {
        std::memset(this, 0, sizeof(BoardState));
    }

    void init_board(BoardState &board)
    {
        board = BoardState();
    }

    void reset_board(BoardState &board)
    {
        board = BoardState();
    }

    uint8_t piece_at(const BoardState &board, uint8_t square)
    {
        if (board.occupied & (1ULL << square))
        {
            for (uint8_t color = 0; color < 2; ++color)
            {
                for (uint8_t type = 0; type < 6; ++type)
                {
                    if (board.pieces_bb[type] & (1ULL << square) && board.colors_bb[color] & (1ULL << square))
                    {
                        return make_piece(static_cast<PieceType>(type), static_cast<Color>(color));
                    }
                }
            }
        }
        return make_piece(NONE, WHITE);
    }

    void place_piece(BoardState &board, uint8_t square, uint8_t piece)
    {

        Bitboard mask = 1ULL << square;
        PieceType type = piece_type(piece);
        Color color = piece_color(piece);

        board.pieces_bb[type] |= mask;
        board.colors_bb[color] |= mask;
        board.occupied |= mask;
    }

    void remove_piece(BoardState &board, uint8_t square)
    {

        Bitboard mask = 1ULL << square;
        uint8_t piece = piece_at(board, square);
        PieceType type = piece_type(piece);
        Color color = piece_color(piece);

        board.pieces_bb[type] &= ~mask;
        board.colors_bb[color] &= ~mask;
        board.occupied &= ~mask;
    }

    void make_move(BoardState &board, Move move)
    {

        uint8_t from = move_from(move);
        uint8_t to = move_to(move);
        uint8_t flags = move_flags(move);
        uint8_t piece = piece_at(board, from);

        Color color = piece_color(piece);
        PieceType type = piece_type(piece);
        Color opp_color = (color == WHITE) ? BLACK : WHITE;

        int from_rank = from / 8;
        int from_file = from % 8;
        int to_rank = to / 8;
        int to_file = to % 8;

        uint8_t target_piece = piece_at(board, to);

        UndoInfo info;
        info.move = move;
        info.captured_piece = target_piece;
        info.castling_rights = board.castling_rights;
        info.en_passant_file = board.en_passant_file;
        info.halfmove_clock = board.halfmove_clock;
        info.hash = board.hash;

        board.move_stack.push(info);

        if (target_piece != make_piece(NONE, WHITE))
        {
            remove_piece(board, to);
        }

        else if (type == PAWN && to_file == board.en_passant_file && ((color == WHITE && to_rank == 5) || (color == BLACK && to_rank == 2)))
        {
            uint8_t captured_square = (color == WHITE) ? to - 8 : to + 8;
            remove_piece(board, captured_square);
        }

        remove_piece(board, from);

        if (type == KING && abs(to_file - from_file) == 2)
        {
            if (to_file > from_file)
            {
                uint8_t rook_from = from + 3;
                uint8_t rook_to = from + 1;
                uint8_t rook_piece = make_piece(ROOK, color);
                remove_piece(board, rook_from);
                place_piece(board, rook_to, rook_piece);
            }
            else
            {
                uint8_t rook_from = from - 4;
                uint8_t rook_to = from - 1;
                uint8_t rook_piece = make_piece(ROOK, color);
                remove_piece(board, rook_from);
                place_piece(board, rook_to, rook_piece);
            }
        }

        bool is_promotion = ((type == PAWN) && (color == WHITE && to >= 56) || (color == BLACK && to <= 7));

        if (is_promotion)
        {
            uint8_t promotion_type = move_promotion(move);
            uint8_t promotion_piece = make_piece(static_cast<PieceType>(promotion_type), color);
            place_piece(board, to, promotion_piece);
        }
        else
        {
            place_piece(board, to, piece);
        }

        if (type == PAWN && abs(to - from) == 16)
        {
            board.en_passant_file = from_file;
        }
        else
        {
            board.en_passant_file = 8;
        }

        if (type == KING)
        {
            if (color == WHITE)
                board.castling_rights &= ~(CASTLE_WHITE_KING | CASTLE_WHITE_QUEEN);
            else
                board.castling_rights &= ~(CASTLE_BLACK_KING | CASTLE_BLACK_QUEEN);
        }
        if (type == ROOK)
        {
            if (from == 0)
                board.castling_rights &= ~CASTLE_WHITE_QUEEN;
            if (from == 7)
                board.castling_rights &= ~CASTLE_WHITE_KING;
            if (from == 56)
                board.castling_rights &= ~CASTLE_BLACK_QUEEN;
            if (from == 63)
                board.castling_rights &= ~CASTLE_BLACK_KING;
        }

        if (target_piece != make_piece(NONE, WHITE) && piece_type(target_piece) == ROOK)
        {
            if (from == 0)
                board.castling_rights &= ~CASTLE_WHITE_QUEEN;
            if (from == 7)
                board.castling_rights &= ~CASTLE_WHITE_KING;
            if (from == 56)
                board.castling_rights &= ~CASTLE_BLACK_QUEEN;
            if (from == 63)
                board.castling_rights &= ~CASTLE_BLACK_KING;
        }

        board.side_to_move = opp_color;
        board.hash = compute_hash(board);
    }

    void unmake_move(BoardState &board, Move move)
    {

        UndoInfo info = board.move_stack.pop();

        Move move = info.move;
        uint8_t from = move_from(move);
        uint8_t to = move_to(move);
        uint8_t piece = piece_at(board, to);
        Color color = piece_color(piece);

        remove_piece(board, to);
        place_piece(board, from, piece);

        if (info.captured_piece != make_piece(NONE, WHITE))
            place_piece(board, to, info.captured_piece);

        board.castling_rights = info.castling_rights;
        board.en_passant_file = info.en_passant_file;
        board.halfmove_clock = info.halfmove_clock;
        board.hash = info.hash;

        board.side_to_move = opposite_color(color);
    }

    Bitboard get_pawn_attacks(uint8_t square, Color color)
    {

        Bitboard attacks = 0;
        Bitboard bb = 1ULL << square;

        if (color == WHITE)
        {
            if ((bb & 0xFEFEFEFEFEFEFEFEULL) != 0)
                attacks |= bb << 7;
            if ((bb & 0x7F7F7F7F7F7F7F7FULL) != 0)
                attacks |= bb << 9;
        }
        else
        {
            if ((bb & 0xFEFEFEFEFEFEFEFEULL) != 0)
                attacks |= bb >> 9;

            if ((bb & 0x7F7F7F7F7F7F7F7FULL) != 0)
                attacks |= bb >> 7;
        }

        return attacks;
    }

    Bitboard get_knight_attacks(uint8_t square)
    {

        Bitboard bb = 1ULL << square;
        Bitboard attacks = 0;

        attacks |= (bb & 0xFEFEFEFEFEFEFEFEULL) << 15;
        attacks |= (bb & 0xFCFCFCFCFCFCFCFCULL) << 6;
        attacks |= (bb & 0x7F7F7F7F7F7F7F7FULL) << 17;
        attacks |= (bb & 0x3F3F3F3F3F3F3F3FULL) << 10;

        attacks |= (bb & 0x7F7F7F7F7F7F7F7FULL) >> 15;
        attacks |= (bb & 0x3F3F3F3F3F3F3F3FULL) >> 6;
        attacks |= (bb & 0xFEFEFEFEFEFEFEFEULL) >> 17;
        attacks |= (bb & 0xFCFCFCFCFCFCFCFCULL) >> 10;

        return attacks;
    }

    Bitboard get_king_attacks(uint8_t square)
    {

        Bitboard bb = 1ULL << square;
        Bitboard attacks = 0;

        attacks |= (bb & 0xFEFEFEFEFEFEFEFEULL) >> 1;
        attacks |= (bb & 0x7F7F7F7F7F7F7F7FULL) << 1;

        attacks |= bb << 8;
        attacks |= bb >> 8;

        attacks |= (bb & 0xFEFEFEFEFEFEFEFEULL) << 7;
        attacks |= (bb & 0x7F7F7F7F7F7F7F7FULL) << 9;
        attacks |= (bb & 0xFEFEFEFEFEFEFEFEULL) >> 9;
        attacks |= (bb & 0x7F7F7F7F7F7F7F7FULL) >> 7;

        return attacks;
    }

    Bitboard get_bishop_attacks(uint8_t square, Bitboard occupied)
    {

        Bitboard attacks = 0;
        int rank = square / 8;
        int file = square % 8;

        for (int upright_rank = rank + 1, upright_file = file + 1; upright_rank < 8 && upright_file < 8; upright_rank++, upright_file++)
        {
            int sq = upright_rank * 8 + upright_file;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        for (int upleft_rank = rank + 1, upleft_file = file - 1; upleft_rank < 8 && upleft_file >= 0; upleft_rank++, upleft_file--)
        {
            int sq = upleft_rank * 8 + upleft_file;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        for (int downright_rank = rank - 1, downright_file = file + 1; downright_rank >= 0 && downright_file < 8; downright_rank--, downright_file++)
        {
            int sq = downright_rank * 8 + downright_file;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        for (int downleft_rank = rank - 1, downleft_file = file - 1; downleft_rank >= 0 && downleft_file >= 0; downleft_rank--, downleft_file--)
        {
            int sq = downleft_rank * 8 + downleft_file;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        return attacks;
    }

    Bitboard get_rook_attacks(uint8_t square, Bitboard occupied)
    {

        Bitboard attacks = 0;
        int rank = square / 8;
        int file = square % 8;

        for (int north = rank + 1; north < 8; north++)
        {
            int sq = north * 8 + file;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        for (int south = rank - 1; south >= 0; south--)
        {
            int sq = south * 8 + file;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        for (int east = file + 1; east < 8; east++)
        {
            int sq = rank * 8 + east;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        for (int west = file - 1; west >= 0; west--)
        {
            int sq = rank * 8 + west;
            attacks |= 1ULL << sq;
            if (occupied & (1ULL << sq))
                break;
        }

        return attacks;
    }

    Bitboard get_queen_attacks(uint8_t square, Bitboard occupied)
    {
        return get_bishop_attacks(square, occupied) | get_rook_attacks(square, occupied);
    }

    bool is_square_attacked(const BoardState &board, uint8_t square, Color by_color)
    {

        if (get_pawn_attacks(square, by_color) & board.pieces_bb[PAWN] & board.colors_bb[by_color])
            return true;

        if (get_knight_attacks(square) & board.pieces_bb[KNIGHT] & board.colors_bb[by_color])
            return true;

        if ((get_bishop_attacks(square, board.occupied) & (board.pieces_bb[BISHOP] | board.pieces_bb[QUEEN])) & board.colors_bb[by_color])
            return true;

        if ((get_rook_attacks(square, board.occupied) & (board.pieces_bb[ROOK] | board.pieces_bb[QUEEN])) & board.colors_bb[by_color])
            return true;

        if (get_king_attacks(square) & board.pieces_bb[KING] & board.colors_bb[by_color])
            return true;

        return false;
    }

    bool is_in_check(const BoardState &board)
    {

        Bitboard king_bb = board.pieces_bb[KING] & board.colors_bb[board.side_to_move];
        if (!king_bb)
            return false;

        uint8_t king_square = lsb(king_bb);
        Color opp_color = (board.side_to_move == WHITE) ? BLACK : WHITE;

        return is_square_attacked(board, king_square, opp_color);
    }

    uint64_t compute_hash(const BoardState &board)
    {

        uint64_t h = 0;

        for (uint8_t type = 0; type < 6; ++type)
        {
            Bitboard bb = board.pieces_bb[type];
            while (bb)
            {
                int sq = lsb(bb);
                Color c = (board.colors_bb[WHITE] & (1ULL << sq)) ? WHITE : BLACK;
                h ^= ZOBRIST_PIECES[type + (c * 6)][sq];
                bb &= bb - 1;
            }
        }

        for (int i = 0; i < 4; ++i)
        {
            if (board.castling_rights & (1 << i))
                h ^= ZOBRIST_CASTLING[i];
        }

        if (board.en_passant_file < 8)
            h ^= ZOBRIST_EN_PASSANT[board.en_passant_file];

        if (board.side_to_move == BLACK)
            h ^= ZOBRIST_SIDE;

        return h;
    }

    int pop_count(Bitboard bb)
    {
        int count = 0;
        while (bb)
        {
            count += bb & 1;
            bb >>= 1;
        }
        return count;
    }

    int lsb(Bitboard bb)
    {
        if (bb == 0)
            return -1;
        int count = 0;
        while ((bb & 1) == 0)
        {
            count++;
            bb >>= 1;
        }
        return count;
    }

    int msb(Bitboard bb)
    {
        if (bb == 0)
            return -1;
        int count = 0;
        while (bb)
        {
            count++;
            bb >>= 1;
        }
        return count - 1;
    }

    Bitboard pop_lsb(Bitboard &bb)
    {
        Bitboard lsb_square = bb & -bb;
        bb ^= lsb_square;
        return lsb_square;
    }

} // namespace chess
