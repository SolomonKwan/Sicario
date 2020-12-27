
#include <iostream>
#include "fen.hpp"

/**
 * Splits a string by a delimeter string and returns vector of results.
 * 
 * @param input: String to split.
 * @param delim: String delimiter.
 * 
 * @return: Vector of input split by delim.
 */
std::vector<std::string> split(std::string input, std::string delim) {
    std::vector<std::string> result;
    std::size_t pos;

    while ((pos = input.find(delim)) != std::string::npos) {
        result.push_back(input.substr(0, pos));
        input = input.substr(pos + delim.length(), input.length() - 1);
    }
    result.push_back(input.substr(0, pos));

    return result;
}
