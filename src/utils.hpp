#pragma once

#include "constants.hpp"
#include <vector>
#include <string>
#include <cassert>

/**
 * @brief Splits a string by a delimeter string and returns vector of results (tokens) without empty strings.
 *
 * @param input String to split.
 * @param delimeter String delimiter.
 * @return Vector of input split by delimeter ignoring empty strings.
 */
std::vector<std::string> split(std::string input, std::string delimeter);

/**
 * @brief Concatentate the vector of strings, split by delimeter.
 *
 * @param strings Vector of strings to concatenate.
 * @param delimeter The delimeter to separate the strings by.
 * @return The concatenated string.
 */
std::string concat(std::vector<std::string> strings, std::string delimeter);

/**
 * @brief Checks if a string is a non-negative integer.
 *
 * @param str String to check.
 * @return True if the string is a non-negative integer, else false (if any characters are not numbers).
 */
bool isNonNegativeInteger(const std::string str);

/**
 * @brief Checks if a string is a positive integer.
 *
 * @param str String to check.
 * @return True if the string is a positive integer, else false (if any characters are not numbers).
 */
bool isPositiveInteger(const std::string str);

/**
 * @brief Checks if a string is of type Move. Does not check if the move is a valid or legal move. Only checks for valid
 * start and end squares and valid promotion option (if present).
 *
 * @param str String to check.
 * @return True if the string is of type Move. Else, false.
 */
bool isMove(const std::string str);

/**
 * @brief Check if the char is a valid rank character. i.e., one of '1', '2', '3', '4', '5', '6', '7', or '8'.
 *
 * @param c Char to check.
 * @return True if valid. Else false.
 */
bool isRank(const char c);

/**
 * @brief Check if the char is a valid file character. i.e., one of 'a', 'b', 'c', 'd', 'e', 'f', 'g', or 'h'.
 *
 * @param c Char to check.
 * @return True if valid. Else false.
 */
bool isFile(const char c);

/**
 * @brief Check if the char is a valid poromotion character. i.e., one of 'q', 'r', 'b', or 'n'.
 *
 * @param c Char to check.
 * @return True, if valid. Else false.
 */
bool isPromotion(const char c);

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
	return static_cast<Square>((move >> DEST_SHIFT) & MOVE_MASK);
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
 * @brief Get the Move object from the string of the move in algebraic notation form. Note, this does not do any checks
 * to see if the move is a castling or en passant move (and therefore does not mark any of the bits to represent this).
 * It does, however, set the promotion move type and promotion piece if present in the string.
 *
 * @param move The move to convert to a Move object.
 * @return Move object.
 */
Move getMove(const std::string& move);

/**
 * @brief Get the string representation of the move in algebraic notation. If moreInfo is true, the format returned is
 *
 *
 * @param move The move to convert to a string.
 * @param moreInfo Whether or not to print extra information about the move type and promotion.
 * @return String representation of the move.
 */
std::string getMove(const Move& move, bool moreInfo = false);

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
 * @brief Get the result string. Either "1-0", "0-1", or "1/2-1/2".
 *
 * @param code End of game ExitCode value.
 * @return String fo the game result.
 */
std::string getResult(const ExitCode& code);

/**
 * @brief Get the next available name of the file in the format of NN_MODEL + "_iteration_n" + ".txt" where "n" is the
 * next available positive integer.
 *
 * @param prefix Prefix of the file location without a trailing "/".
 * @param numSimulations Number of simulation in this file.
 * @return String of the next available name of the file in the format of NN_MODEL + "_iteration_1" + ".txt"
 */
std::string getDataLabel(std::string prefix, int numSimulations);

/**
 * @brief Calculates the ranks of the numbers in an array of length N. The smallest number has the highest rank (N),
 * while the largest number has the lowest rank (1). Does not consider duplicate elements.
 *
 * @param vec Array of floats to be ranked.
 * @return A vector corresponding to each elements rank.
 */
std::vector<size_t> rankSort(const std::vector<float>& vec);

/**
 * @brief Get the corresponding square on the vertically opposite side of the board. E.g., mirror(A1) = A8.
 *
 * @param square Square to mirror.
 * @return Square Vertically corresponding square.
 */
Square mirror(Square square);

/**
 * @brief Get a random integer between 0 and 100 (inclusive).
 *
 * @return A random integer between 0 and 100 (inclusive).
 */
int randInt();

/**
 * @brief Returns a copy of the string with leading and trailing whitepaces removed.
 *
 * @param string The string to trim.
 * @return A copy of the string with the leading and the trailing whitespaces removed.
 */
std::string trim(std::string string);

/**
 * @brief Concatenates the FEN substrings into a single string.
 *
 * @param strings Substrings of the FEN string.
 * @param delim The delimeter string. Defaulted to a space.
 * @return Complete FEN string.
 */
std::string concatFEN(const std::vector<std::string> strings, const std::string delim = " ");

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

/**
 * @brief Zeroes out the bit in the given number.
 *
 * @tparam T Data type of number to alter.
 * @param num The num to modify.
 * @param index The index to zero out.
 */
template<typename T>
inline void zeroBit(T& num, const int index) {
	num &= ~(static_cast<T>(1) << index);
}

/**
 * @brief Sets the bit in the given number.
 *
 * @tparam T Data type of number to alter.
 * @param num The number to modify.
 * @param index The index to set.
 */
template<typename T>
inline void setBit(T& num, const int index) {
	num |= static_cast<T>(1) << index;
}

/**
 * @brief Zeroes out the given start and sets the given end bit in the given number. Assumes a priori that the start bit
 * is already set and that the end bit is not already set.
 *
 * @tparam T Data type of number to alter.
 * @param num The number to modify.
 * @param start The index to zero out.
 * @param end The index to set.
 */
template<typename T>
inline void zeroAndSetBit(T& num, const int start, const int end) {
	assert(isSet<T>(num, start) == true);
	assert(isSet<T>(num, end) == false);
	num ^= static_cast<T>(1) << start | static_cast<T>(1) << end;
}

template<typename T>
inline Bitboard extractBit(T number, uint index) {
	return static_cast<Bitboard>((number >> index) & static_cast<T>(1));
}