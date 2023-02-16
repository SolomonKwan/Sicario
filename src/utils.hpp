#ifndef UTILS_HPP
#define UTILS_HPP

#include "constants.hpp"
#include <vector>
#include <string>
#include <cassert>

/**
 * @brief Splits a string by a delimeter string and returns vector of results without empty strings.
 *
 * @param input String to split.
 * @param delim String delimiter.
 * @return Vector of input split by delim ignoring empty strings.
 */
std::vector<std::string> split(std::string input, std::string delim);

/**
 * @brief Concatentate the vector of strings, split by delimeter.
 *
 * @param strings Vector of strings to concatenate.
 * @param delimeter The delimeter to separate the strings by.
 * @return The concatenated string.
 */
std::string concat(std::vector<std::string> strings, std::string delimeter);

/**
 * @brief Checks if a string is a positive integer.
 *
 * @param str String to check.
 * @return True if the string is a positive integer, else false (if any characters are not numbers).
 */
bool isPostiveInteger(std::string str);

/**
 * @brief Prints the bitboard in white's view. LSB is a1 and 2nd LSB is h1. 9th LSB is a2 and so on.
 *
 * @param position A bitboard.
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
inline MoveClass type(Move move) {
	return static_cast<MoveClass>(move & (MOVE_TYPE_MASK << MOVE_TYPE_SHIFT));
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

/**
 * @brief Get the promotion string.
 *
 * @param promo Promotion type.
 * @return String of the promotion piece.
 */
std::string getPromoString(Promotion promo);

/**
 * @brief Get the move type string.
 *
 * @param promo Move type.
 * @return String of the move type.
 */
std::string getTypeString(MoveClass type);

/**
 * @brief Convert a string in algebraic move format to a Move object and return it.
 *
 * @param string Move string in algebraic notation.
 * @return Move object.
 */
Move getMovefromAlgebraic(std::string string);

/**
 * @brief Calculates the ranks of the numbers in an array of length N. The smallest number has the highest rank (N),
 * while the largest number has the lowest rank (1). Does not consider duplicate elements.
 *
 * @param vec Array of floats to be ranked.
 * @return A vector corresponding to each elements rank.
 */
std::vector<size_t> rankSort(const std::vector<float>& vec);

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

#endif