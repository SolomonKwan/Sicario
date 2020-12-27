
#ifndef FEN_HPP
#define FEN_HPP

#include "constants.hpp"
#include "game.hpp"

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
