#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../src/constants.hpp"
#include "../src/movegen.hpp"
#include "../src/utils.hpp"

#define GREEN = "\033[0;32m"
#define RED = "\033[0;31m"
#define NORMAL = "\033[0m"

enum TestType {
    ROOK_REACH_INDEX,
};

std::unordered_map<TestType, int> TESTS_COUNTS = {
    {ROOK_REACH_INDEX, 0}
};

std::unordered_map<TestType, std::string> TESTS_NAMES = {
    {ROOK_REACH_INDEX, "getRookReachIndex"}
};

int TOTAL_TEST_COUNT = 0;
int TESTS_PASSED = 0;
int TESTS_FAILED = 0;

template <typename T>
void assertEquals(TestType testType, T expected, T actual) {
    TOTAL_TEST_COUNT++;
    TESTS_COUNTS[testType]++;
    std::string testName = TESTS_NAMES[testType] + std::to_string(TESTS_COUNTS[testType]);
    if (actual == expected) {
        TESTS_PASSED++;
        std::cout << testName  << "\t[ \033[0;32mPASSED\033[0m ]" << '\n';
    } else {
        TESTS_FAILED++;
        std::cout << testName << "\t[ \033[0;31mFAILED\033[0m ]\t";
        std::cout << "Expected " << expected << "\tGot " << actual << '\n';
    }
}

const std::array<std::vector<int>, 64> ROOK = computeRookIndices();

int rookReachIndex(Bitboard occupancy, Square square) {
    return ROOK[square][getRookReachIndex(occupancy, square)];
}

Bitboard generatePos(std::vector<Square> squares) {
    Bitboard pos = 0ULL;
    for (Square square : squares) {
        pos |= 1ULL << square;
    }
    return pos;
}

void run_getRookReachIndex_tests() {
    Square square = A1;
    Bitboard pos1 = generatePos({A4, C1});
    Bitboard pos2 = generatePos({A4, A5, A6, C1, G1, D1});
    Bitboard pos3 = generatePos({A4, A7, C1, D1, E1, F1});
    Bitboard pos4 = generatePos({A4, A5, A6, A7, C1, D1, E1, F1, G1});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    square = A2;
    pos1 = generatePos({A4, C2});
    pos2 = generatePos({A4, C2, G2, D2, A5, A6});
    pos3 = generatePos({A4, C2, D2, E2, F2, A7});
    pos4 = generatePos({A4, A5, A6, A7, C2, D2, E2, F2, G2});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    square = A3;
    pos1 = generatePos({A4, C3, A2});
    pos2 = generatePos({A4, C3, G3, D3, A5, A6, A2});
    pos3 = generatePos({A4, C3, D3, E3, F3, A7, A2});
    pos4 = generatePos({A2, A4, A5, A6, A7, C3, D3, E3, F3, G3});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    square = A4;
    pos1 = generatePos({A2, A5, C4});
    pos2 = generatePos({A2, A5, A6, C4, F4, G4});
    pos3 = generatePos({A2, A5, A6, C4, D4, F4, G4});
    pos4 = generatePos({A2, A5, A6, C4, D4, E4, F4, G4});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    square = B1;
    pos1 = generatePos({B4, C1});
    pos2 = generatePos({B4, B5, B6, C1, G1, D1});
    pos3 = generatePos({B4, B7, C1, D1, E1, F1});
    pos4 = generatePos({B4, B5, B6, B7, C1, D1, E1, F1, G1});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    // square = A2;
    // pos1 = generatePos({A4, C2});
    // pos2 = generatePos({A4, C2, G2, D2, A5, A6});
    // pos3 = generatePos({A4, C2, D2, E2, F2, A7});
    // pos4 = generatePos({A4, A5, A6, A7, C2, D2, E2, F2, G2});
    // assertEquals<int>("getRookReachIndex4", rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    // assertEquals<int>("getRookReachIndex5", rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    // assertEquals<int>("getRookReachIndex6", rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    // square = A3;
    // pos1 = generatePos({A4, C3, A2});
    // pos2 = generatePos({A4, C3, G3, D3, A5, A6, A2});
    // pos3 = generatePos({A4, C3, D3, E3, F3, A7, A2});
    // pos4 = generatePos({A2, A4, A5, A6, A7, C3, D3, E3, F3, G3});
    // assertEquals<int>("getRookReachIndex7", rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    // assertEquals<int>("getRookReachIndex8", rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    // assertEquals<int>("getRookReachIndex9", rookReachIndex(pos3, square), rookReachIndex(pos4, square));

    // square = A4;
    // pos1 = generatePos({A2, A5, C4});
    // pos2 = generatePos({A2, A5, A6, C4, F4, G4});
    // pos3 = generatePos({A2, A5, A6, C4, D4, F4, G4});
    // pos4 = generatePos({A2, A5, A6, C4, D4, E4, F4, G4});
    // assertEquals<int>("getRookReachIndex10", rookReachIndex(pos1, square), rookReachIndex(pos2, square));
    // assertEquals<int>("getRookReachIndex11", rookReachIndex(pos2, square), rookReachIndex(pos3, square));
    // assertEquals<int>("getRookReachIndex12", rookReachIndex(pos3, square), rookReachIndex(pos4, square));
}

void run_getRookMovesIndex_tests() {

}

void run_getBishopReachIndex_tests() {

}

void run_getBishopMovesIndex_tests() {

}

void printFinalResult() {
    std::cout << "\nPASSED: \033[0;32m" << std::to_string(TESTS_PASSED) << "\033[0m\t";
    std::cout << "FAILED: \033[0;31m" << std::to_string(TESTS_FAILED) << "\033[0m\n";
    std::cout << "TOTAL: " << std::to_string(TOTAL_TEST_COUNT) << '\n';
}

int main(int argc, char const *argv[]) {
    run_getRookReachIndex_tests();
    run_getRookMovesIndex_tests();
    run_getBishopReachIndex_tests();
    run_getBishopMovesIndex_tests();
    printFinalResult();
    return 0;
}