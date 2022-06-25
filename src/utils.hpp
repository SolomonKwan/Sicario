#ifndef UTILS_HPP
#define UTILS_HPP

#include "constants.hpp"
#include <vector>
#include <string>

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
    return bitboard && !(bitboard & (bitboard - 1));
}

#endif