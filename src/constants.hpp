#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <vector>
#include <array>

const std::string NAME = "Sicario";
const std::string CODENAME = "Iteration-3";
const std::string VERSION = "v0.3.0";
const std::string AUTHOR = "S. Kwan";
const std::string CHESS_PROGRAMMING = "www.chessprogramming.org";
const std::string STOCKFISH = "Stockfish";
const std::string BLUE_FEVER_SOFT = "bluefeversoft";
const std::string STANDARD_GAME = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const uint PLAYER_COUNT = 2;
const uint SQUARE_COUNT = 64;
const uint FILE_COUNT = 8;
const uint RANK_COUNT = 8;
const uint PIECE_TYPE_COUNT = 12;
const uint CASTLING_OPTIONS = 4;
const uint CASTLING_COMBOS = 16;
const uint DESTINATION_SHIFT = 6;
const uint MOVE_TYPE_SHIFT = 12;
const uint PROMOTION_SHIFT = 14;
const uint KING_INDEX = 0;
const uint MOVESET_SIZE = 32;
const uint MAX_MOVES = 500;
const uint DEFAULT_HASH_SIZE = 16;
const uint MAX_PIECE_COUNT = 10;
const uint FEN_BOARD_INDEX = 0;
const uint FEN_MOVE_INDEX = 1;
const uint FEN_CASTLING_INDEX = 2;
const uint FEN_EN_PASSANT_INDEX = 3;
const uint FEN_HALFMOVE_INDEX = 4;
const uint FEN_FULLMOVE_INDEX = 5;

const uint16_t MOVE_MASK = 0b111111;
const uint16_t MOVE_TYPE_MASK = 0b11;
const uint16_t PROMOTION_MASK = 0b11;

typedef uint64_t Bitboard;
const Bitboard DARK_BB = 0xAA55AA55AA55AA55ULL;
const Bitboard ZERO_BB = 0ULL;
const Bitboard ONE_BB = 1ULL;
const Bitboard MAX_BB = 0xFFFFFFFFFFFFFFFF;

typedef uint16_t Move;
typedef std::vector<Move> MoveVector;
typedef std::vector<MoveVector> MoveFamily;
typedef std::array<MoveFamily, SQUARE_COUNT> MoveFamilies;
typedef std::vector<Move> const* MoveSet;
typedef uint64_t Hash;
typedef std::vector<Bitboard> BitboardVector;
typedef std::array<std::vector<Bitboard>, SQUARE_COUNT> BitboardFamily;
typedef std::array<std::vector<uint>, SQUARE_COUNT> IndicesFamily;

enum File { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };

enum Rank { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

enum Direction {
    N = 8,
    NE = 9,
    E = 1,
    SE = -7,
    S = -8,
    SW = -9,
    W = -1,
    NW = 7,

    NNE = +17,
    ENE = +10,
    ESE = -6,
    SES = -15,
    SWS = -17,
    WSW = -10,
    WNW = +6,
    NWN = +15,
};

enum Player { BLACK, WHITE };

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

enum PieceType {
    B_KING, W_KING,
    W_QUEEN, W_ROOK, W_BISHOP, W_KNIGHT, W_PAWN,
    B_QUEEN, B_ROOK, B_BISHOP, B_KNIGHT, B_PAWN,
    NO_PIECE
};

enum BasePieceType {
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

enum Castling {
    WKSC, WQSC, BKSC, BQSC
};

enum MoveType {
    NORMAL = 0,
    PROMOTION = 1 << MOVE_TYPE_SHIFT,
    EN_PASSANT = 2 << MOVE_TYPE_SHIFT,
    CASTLING = 3 << MOVE_TYPE_SHIFT
};

enum Promotion {
    pKNIGHT = 0,
    pBISHOP = 1 << PROMOTION_SHIFT,
    pROOK = 2 << PROMOTION_SHIFT,
    pQUEEN = 3 << PROMOTION_SHIFT
};

enum ExitCode {
    NORMAL_PLY,
    WHITE_WINS,
    BLACK_WINS,
    STALEMATE,
    THREE_FOLD_REPETITION,
    FIFTY_MOVES_RULE,
    INSUFFICIENT_MATERIAL,
};

inline Square operator+(const Square& square, const Direction& direction) { // CHECK Why do args have to be const?
    return static_cast<Square>(static_cast<uint>(square) + static_cast<uint>(direction));
}

inline Direction operator+(const Direction& direction1, const Direction& direction2) { // CHECK Why do args have to be const?
    return static_cast<Direction>(static_cast<uint>(direction1) + static_cast<uint>(direction2));
}

inline Square operator++(Square& square) { // Pre-fix
    square = static_cast<Square>(static_cast<uint>(square) + 1U);
    return square;
}

inline Square operator++(Square& square, int) { // Post-fix
    Square prev = square;
    square = static_cast<Square>(static_cast<uint>(square) + 1U);
    return prev;
}

inline File operator++(File& file) { // Pre-fix
    file = static_cast<File>(static_cast<uint>(file) + 1U);
    return file;
}

inline File operator++(File& file, int) { // Post-fix
    File prev = file;
    file = static_cast<File>(static_cast<uint>(file) + 1U);
    return prev;
}

inline Rank operator--(Rank& rank, int) {
    Rank prev = rank;
    rank = static_cast<Rank>(static_cast<uint>(rank) - 1U);
    return prev;
}

inline Player operator!(const Player& player) {
    return static_cast<Player>(1U - player);
}

const std::string promoName[4] = {
    "n", "b", "r", "q"
};

const std::string moveName[4] = {
    "Normal", "Promotion", "En-passant", "Castling"
};

const std::string piece_type_string[PIECE_TYPE_COUNT + 1] = {
    "B_KING", "W_KING",
    "W_QUEEN", "W_ROOK", "W_BISHOP", "W_KNIGHT", "W_PAWN",
    "B_QUEEN", "B_ROOK", "B_BISHOP", "B_KNIGHT", "B_PAWN",
    "NO_PIECE"
};

const std::string squareName[SQUARE_COUNT + 1] = {
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

#endif