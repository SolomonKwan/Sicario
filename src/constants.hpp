
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

#define NAME "Sicario"
#define CODENAME "iteration-2"
#define VERSION "v0.2.0"
#define AUTHOR "S. Kwan"
#define CHESS_PROGRAMMING "www.chessprogramming.org"
#define STOCKFISH "Stockfish"
#define BLUE_FEVER_SOFT "bluefeversoft"
#define STANDARD_GAME "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define DARK 0xAA55AA55AA55AA55

typedef uint64_t Hash;
typedef uint64_t Bitboard;
typedef uint16_t Move;

/**
 * Player side enum.
 */
enum Player {
    BLACK, WHITE // TODO Maybe change this to another type so that we can do logical operations without casting.
};

// inline Player operator!(const Player& player) {
//     return !player;
// }

/**
 * The squares of the board and their associated numbers. NONE (64) used as a sentinel value.
 */
enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NONE
};

/**
 * The squares in string format. Used for development and debugging.
 */
const std::string squareName[65] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "None"
};

/**
 * The piece types.
 */
enum PieceType {
    B_KING, W_KING,
    W_QUEEN, W_ROOK, W_BISHOP, W_KNIGHT, W_PAWN,
    B_QUEEN, B_ROOK, B_BISHOP, B_KNIGHT, B_PAWN,
    NO_PIECE
};

/**
 * Piece type strings. Used for development and debugging.
 */
const std::string piece_type_string[13] = {
    "B_KING", "W_KING",
    "W_QUEEN", "W_ROOK", "W_BISHOP", "W_KNIGHT", "W_PAWN",
    "B_QUEEN", "B_ROOK", "B_BISHOP", "B_KNIGHT", "B_PAWN",
    "NO_PIECE"
};

/**
 * The promotion piece types. In the format for move hashes (i.e. left shifted by 14 bits).
 */
enum Promotion {
    pKNIGHT = 0, pBISHOP = 1 << 14, pROOK = 2 << 14, pQUEEN = 3 << 14
};

/**
 * Promotion piece strings.
 */
const std::string promoName[4] = {
    "n", "b", "r", "q"
};

/**
 * The move types. In the format for move hashes (i.e. left shifted by 12 bits).
 */
enum MoveType {
    NORMAL = 0, PROMOTION = 1 << 12, EN_PASSANT = 2 << 12, CASTLING = 3 << 12
};

/**
 * Move type strings.
 */
const std::string moveName[4] = {
    "Normal", "Promotion", "En-passant", "Castling"
};

/**
 * Exit codes for the game or parsing.
 */
enum ExitCode {
    NORMAL_PLY,
    WHITE_WINS,
    BLACK_WINS,
    STALEMATE,
    THREE_FOLD_REPETITION,
    FIFTY_MOVES_RULE,
    INSUFFICIENT_MATERIAL,
    DRAW_BY_AGREEMENT,
};

/**
 * Player types.
 */
enum PlayerType {
    HUMAN, COMPUTER
};

/**
 * Castling options.
 */
enum Castling {
    WKSC, WQSC, BKSC, BQSC
};

#endif
