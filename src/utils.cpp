#include <bitset>
#include <iostream>
#include <algorithm>

#include "utils.hpp"
#include "constants.hpp"

std::vector<std::string> split(std::string input, const std::string delim) {
	std::vector<std::string> result;
	std::size_t pos;

	while ((pos = input.find(delim)) != std::string::npos) {
		if (input.substr(0, pos) != "") result.push_back(input.substr(0, pos));
		input = input.substr(pos + delim.length(), input.length() - 1);
	}
	if (input.substr(0, pos) != "") result.push_back(input.substr(0, pos));

	return result;
}

std::string concat(const std::vector<std::string> strings, const std::string delimeter) {
	std::string combinedStr = "";
	for (std::string str : strings) {
		combinedStr += str + delimeter;
	}
	combinedStr = combinedStr.substr(0, combinedStr.size() - 1);
	return combinedStr;
}

bool isPostiveInteger(const std::string str) {
	for (char c : str) {
		if (!std::isdigit(c)) return false;
	}
	return true;
}

void displayBB(const uint64_t position) {
	std::string positionString = std::bitset<SQUARE_COUNT>(position).to_string();
	std::cout << "\n";
	for (int i = 0; i < 8; i++) {
		// Reverse each line then print.
		std::string line = positionString.substr(8 * i, 8);
		std::reverse(line.begin(), line.end());
		std::cout << line << '\n';
	}
	std::cout << '\n' << std::flush;
}

void printMove(const Move move, const bool extraInfo, const bool flush) {
	if (extraInfo) {
		std::cout << squareName[start(move)] << squareName[end(move)];
		std::cout << " ";
		std::cout << getTypeString(type(move)) << " " << getPromoString(promo(move));
	} else if (type(move) == PROMOTION) {
		std::cout << squareName[start(move)] << squareName[end(move)] << getPromoString(promo(move));
	} else {
		std::cout << squareName[start(move)] << squareName[end(move)];
	}

	if (flush) std::cout << '\n';
}

std::string getPromoString(const Promotion promo) {
	switch (promo) {
		case pKNIGHT:
			return promoName[0];
		case pBISHOP:
			return promoName[1];
		case pROOK:
			return promoName[2];
		default:
			return promoName[3];
	}
}

std::string getTypeString(const MoveClass type) {
	switch (type) {
		case NORMAL:
			return moveName[0];
		case PROMOTION:
			return moveName[1];
		case EN_PASSANT:
			return moveName[2];
		default:
			return moveName[3];
	}
}