#include <iostream>
#include <string>
#include <vector>
#include "../src/constants.hpp"
#include "../src/movegen.hpp"
#include "../src/utils.hpp"

#define GREEN = "\033[0;32m"
#define RED = "\033[0;31m"
#define NORMAL = "\033[0m"

template <typename T>
void assertEquals(std::string testName, T expected, T actual) {
    if (actual == expected) {
        std::cout << testName << "\t\t[ \033[0;32mPASSED\033[0m ]" << '\n';
    } else {
        std::cout << testName << "\t\t[ \033[0;31mFAILED\033[0m ]\t";
        std::cout << "Expected " << expected << "\tGot " << actual << '\n';
    }
}

void run_getRookReachIndex_tests() {
    const std::array<std::vector<int>, 64> ROOK = computeRookIndices();

    Square square = A1;
    Bitboard pos1 = 1ULL << A4 | 1ULL << C1;
    Bitboard pos2 = 1ULL << A4 | 1ULL << C1 | 1ULL << G1 | 1ULL << D1 | 1ULL << A5 | 1ULL << A6;
    Bitboard pos3 = 1ULL << A4 | 1ULL << C1 | 1ULL << D1 | 1ULL << E1 | 1ULL << F1 | 1ULL << A7;
    Bitboard pos4 = 1ULL << A4 | 1ULL << A5 | 1ULL << A6 | 1ULL << A7 | 1ULL << C1 | 1ULL << D1 | 1ULL << E1 |
            1ULL << F1 | 1ULL << G1;
    assertEquals<int>("getRookReachIndex1", ROOK[square][getRookReachIndex(pos1, square)], ROOK[square][
            getRookReachIndex(pos2, square)]);
    assertEquals<int>("getRookReachIndex2", ROOK[square][getRookReachIndex(pos2, square)], ROOK[square][
            getRookReachIndex(pos3, square)]);
    assertEquals<int>("getRookReachIndex2", ROOK[square][getRookReachIndex(pos3, square)], ROOK[square][
            getRookReachIndex(pos4, square)]);
}

void run_getRookMovesIndex_tests() {

}

void run_getBishopReachIndex_tests() {

}

void run_getBishopMovesIndex_tests() {

}

int main(int argc, char const *argv[]) {
    run_getRookReachIndex_tests();
    run_getRookMovesIndex_tests();
    run_getBishopReachIndex_tests();
    run_getBishopMovesIndex_tests();
    return 0;
}