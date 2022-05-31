#ifndef UTILS_HPP
#define UTILS_HPP

#include "constants.hpp"
#include <vector>
#include <string>

/**
 * Splits a string by a delimeter string and returns vector of results.
 * @param input: String to split.
 * @param delim: String delimiter.
 * @return: Vector of input split by delim.
 */
std::vector<std::string> split(std::string input, std::string delim);

Move parseMove(std::string);

std::string concat(std::vector<std::string> strings, std::string delimeter);

bool isNumber(std::string);

#endif