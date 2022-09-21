#ifndef UTILS_HPP
#define UTILS_HPP

#include "constants.hpp"
#include <vector>
#include <string>
#include <cassert>

/**
 * @brief Splits a string by a delimeter string and returns vector of results without empty strings.
 *
 * @param input: String to split.
 * @param delim: String delimiter.
 * @return: Vector of input split by delim ignoring empty strings.
 */
std::vector<std::string> split(std::string input, std::string delim);

// TODO maybe redo.
Move parseMove(std::string);

/**
 * @brief Concatentate the vector of strings, split by delimeter.
 *
 * @param strings: Vector of strings to concatenate.
 * @param delimeter: The delimeter to separate the strings by.
 * @return: The concatenated string.
 */
std::string concat(std::vector<std::string> strings, std::string delimeter);

/**
 * @brief Checks if a string is a positive integer.
 *
 * @param str: String to check.
 * @return: True if the string is a positive integer, else false (if any characters are not numbers).
 */
bool isPostiveInteger(std::string str);

/**
 * @brief Prints the bitboard in white's view. LSB is a1 and 2nd LSB is h1. 9th LSB is a2 and so on.
 *
 * @param position: A bitboard.
 */
void displayBB(uint64_t position);

/**
 * @brief Checks if a bitboard has only 1 bit set.
 *
 * @param bitboard The bitboard to check.
 * @return True if only 1 bit set, else false.
 */
inline bool oneBitSet(Bitboard bitboard) {
    return bitboard && !(bitboard & (bitboard - ONE_BB));
}

/**
 * @brief Checks if the given square is dark square.
 *
 * @param square The square to check.
 * @return True if the square is dark, else false.
 */
inline bool isDark(Square square) {
    return (DARK_BB >> square) & ONE_BB;
}

/**
 * @brief Get the start square of a move.
 *
 * @param move The move to parse.
 * @return The start square encoded in the move.
 */
inline Square start(Move move) {
    return static_cast<Square>(move & MOVE_MASK);
}

/**
 * @brief Get the end square of a move.
 *
 * @param move The move to parse.
 * @return The end square encoded in the move.
 */
inline Square end(Move move) {
    return static_cast<Square>((move >> DESTINATION_SHIFT) & MOVE_MASK);
}

/**
 * @brief Get the move type of a move.
 *
 * @param move The move to parse.
 * @return The move type encoded in the move.
 */
inline MoveType type(Move move) {
    return static_cast<MoveType>(move & (MOVE_TYPE_MASK << MOVE_TYPE_SHIFT));
}

/**
 * @brief Get the promotion of a move.
 *
 * @param move The move to parse.
 * @return The promotion encoded in the move.
 */
inline Promotion promo(Move move) {
    return static_cast<Promotion>(move & (PROMOTION_MASK << PROMOTION_SHIFT));
}

/**
 * @brief Get the rank of a square.
 *
 * @param move The square of concern.
 * @return The rank of the square.
 */
inline Rank rank(Square square) {
    return static_cast<Rank>(square / RANK_COUNT);
}

/**
 * @brief Get the file of a square.
 *
 * @param move The square of concern.
 * @return The file of the square.
 */
inline File file(Square square) {
    return static_cast<File>(square % FILE_COUNT);
}

/**
 * @brief Print the move for debugging/development purposes.
 *
 * @param move The move to print.
 * @param extraInfo Whether or not to print the extra information relating to the move type and promotion.
 */
void printMove(Move move, bool extraInfo, bool flush = false);

std::string getPromoString(Promotion promo);

std::string getTypeString(MoveType type);

/**
 * @brief Check if the bit at 'index' of 'number' is set.
 *
 * @tparam T Type
 * @param number The number to check for the set bit.
 * @param index Index of the bit to check.
 * @return True if the bit is set, else false.
 */
template<typename T>
inline bool isSet(T number, uint index) {
    return number & (static_cast<T>(1) << index);
}

template<typename N, typename I>
inline void clearBit(N& number, I index) {
    number &= ~(static_cast<N>(1) << index);
}

template<typename N, typename I>
inline void setBit(N& number, I index) {
    number |= static_cast<N>(1) << index;
}

constexpr BasePieceType baseTypeFromPiece(PieceType piece) {
    switch (piece) {
        case W_KING:
        case B_KING:
            return KING;
        case W_QUEEN:
        case B_QUEEN:
            return QUEEN;
        case W_ROOK:
        case B_ROOK:
            return ROOK;
        case W_BISHOP:
        case B_BISHOP:
            return BISHOP;
        case W_KNIGHT:
        case B_KNIGHT:
            return KNIGHT;
        case W_PAWN:
        case B_PAWN:
            return PAWN;
        default: // W_PAWN or B_PAWN
            assert(false);
    }
}

constexpr Player colourFromPieceType(PieceType piece) {
    switch (piece) {
        case W_KING:
        case W_QUEEN:
        case W_ROOK:
        case W_BISHOP:
        case W_KNIGHT:
        case W_PAWN:
            return WHITE;
        case B_KING:
        case B_QUEEN:
        case B_ROOK:
        case B_BISHOP:
        case B_KNIGHT:
        case B_PAWN:
            return BLACK;
        default: // W_PAWN or B_PAWN
            assert(false);
    }
}

inline Square reflectVertical(const Square square) {
    return static_cast<Square>(square ^ 56);
}

#endif