#include "utils.hpp"
#include "constants.hpp"

/**
 * Splits a string by a delimeter string and returns vector of results.
 * @param input: String to split.
 * @param delim: String delimiter.
 * @return: Vector of input split by delim.
 */
std::vector<std::string> split(std::string input, std::string delim) {
    std::vector<std::string> result;
    std::size_t pos;

    while ((pos = input.find(delim)) != std::string::npos) {
        if (input.substr(0, pos) != "") result.push_back(input.substr(0, pos));
        input = input.substr(pos + delim.length(), input.length() - 1);
    }
    if (input.substr(0, pos) != "") result.push_back(input.substr(0, pos));

    return result;
}

Move parseMove(std::string move_str) {
    Move move = 0;
    if (move_str[0] < 'a' || move_str[0] > 'h' || move_str[2] < 'a' || move_str[2] > 'h') {
        return 0;
    }

    if (move_str[1] < '1' || move_str[1] > '8' || move_str[3] < '1' || move_str[3] > '8') {
        return 0;
    }

    int start_file = move_str[0] - 'a';
    int start_rank = move_str[1] - '1';
    int end_file = move_str[2] - 'a';
    int end_rank = move_str[3] - '1';

    if (move_str.length() == 5) {
        move |= PROMOTION;
        if (move_str[4] == 'q') {
            move |= pQUEEN;
        } else if (move_str[4] == 'r') {
            move |= pROOK;
        } else if (move_str[4] == 'b') {
            move |= pBISHOP;
        }
    }

    move |= 8 * start_rank + start_file;
    move |= ((8 * end_rank + end_file) << 6);
    return move;
}

std::string concat(std::vector<std::string> strings, std::string delimeter) {
    std::string combinedStr = "";
    for (std::string str : strings) {
        combinedStr += str + delimeter;
    }
    combinedStr = combinedStr.substr(0, combinedStr.size() - 1);
    return combinedStr;
}