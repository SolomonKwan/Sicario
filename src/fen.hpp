
#ifndef FEN_HPP
#define FEN_HPP

#include "constants.hpp"

/**
 * Return true if a square is a dark square, else false.
 * @param square: The square to check.
 */
bool isDark(int square);

/**
 * Get and return the FEN string of the current position.
 * @param game: Pointer to game struct.
 */
std::string getFEN(Game* game);

/**
 * Splits a string by a delimeter string and returns vector of results.
 * 
 * @param input: String to split.
 * @param delim: String delimiter.
 * 
 * @return: Vector of input split by delim.
 */
std::vector<std::string> split(std::string input, std::string delim);

#endif
