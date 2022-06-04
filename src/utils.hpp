#ifndef UTILS_HPP
#define UTILS_HPP

#include "constants.hpp"
#include <vector>
#include <string>

/**
 * Splits a string by a delimeter string and returns vector of results without empty strings.
 * @param input: String to split.
 * @param delim: String delimiter.
 * @return: Vector of input split by delim ignoring empty strings.
 */
std::vector<std::string> split(std::string input, std::string delim);

// TODO maybe redo.
Move parseMove(std::string);

/**
 * Concatentate the vector of strings, split by delimeter.
 * @param strings: Vector of strings to concatenate.
 * @param delimeter: The delimeter to separate the strings by.
 * @return: The concatenated string.
 */
std::string concat(std::vector<std::string> strings, std::string delimeter);

/**
 * Checks if a string is a positive integer.
 * @param str: String to check.
 * @return: True if the string is a positive integer, else false (if any characters are not numbers).
 */
bool isPostiveInteger(std::string str);

/**
 * Prints the bitboard in white's view. LSB is a1 and 2nd LSB is h1. 9th LSB is a2 and so on.
 * @param position: A bitboard.
 */
void displayBB(uint64_t position);

#endif