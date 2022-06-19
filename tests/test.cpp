#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "../src/constants.hpp"
#include "../src/movegen.hpp"
#include "../src/utils.hpp"

#define GREEN = "\033[0;32m"
#define RED = "\033[0;31m"
#define NORMAL = "\033[0m"

enum TestType {
    ROOK_REACH_INDEX,
    ROOK_MOVES_INDEX,
    BISHOP_REACH_INDEX,
    BISHOP_MOVES_INDEX,
    KNIGHT_MAGIC_NUMS,
};

std::unordered_map<TestType, int> TESTS_COUNTS = {
    {ROOK_REACH_INDEX, 0},
    {ROOK_MOVES_INDEX, 0},
    {BISHOP_REACH_INDEX, 0},
    {BISHOP_MOVES_INDEX, 0},
    {KNIGHT_MAGIC_NUMS, 0},
};

std::unordered_map<TestType, std::string> TESTS_NAMES = {
    {ROOK_REACH_INDEX, "getRookReachIndex"},
    {ROOK_MOVES_INDEX, "getRookMovesIndex"},
    {BISHOP_REACH_INDEX, "getBishopReachIndex"},
    {BISHOP_MOVES_INDEX, "getBishopMovesIndex"},
    {KNIGHT_MAGIC_NUMS, "knightMagicNums"},
};

int TOTAL_TEST_COUNT = 0;
int TESTS_PASSED = 0;
int TESTS_FAILED = 0;

const std::array<std::vector<int>, 64> ROOK_REACH_INDICES = computeRookReachIndices();
const std::array<std::vector<int>, 64> BISHOP_REACH_INDICES = computeBishopReachIndices();

template <typename T>
void assertEquals(TestType testType, T expected, T actual, int testNum) {
    TOTAL_TEST_COUNT++;
    TESTS_COUNTS[testType]++;

    if (testNum != TESTS_COUNTS[testType]) {
        std::cout << "Test number does not match the count - " << TESTS_NAMES[testType];
        std::cout << "\tExpected: " << TESTS_COUNTS[testType] << "\tActual: " << testNum << '\n';
        return;
    }

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

int rookReachIndex(Bitboard occupancy, Square square) {
    return ROOK_REACH_INDICES[square][getRookReachIndex(occupancy, square)];
}

int rookMovesIndex(Bitboard occupancy, Square square) {
    return getRookMovesIndex(occupancy, square);
}

int bishopReachIndex(Bitboard occupancy, Square square) {
    return BISHOP_REACH_INDICES[square][getBishopReachIndex(occupancy, square)];
}

int bishopMovesIndex(Bitboard occupancy, Square square) {
    return getBishopMovesIndex(occupancy, square);
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
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 1);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 2);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 3);

    square = A2;
    pos1 = generatePos({A4, C2});
    pos2 = generatePos({A4, C2, G2, D2, A5, A6});
    pos3 = generatePos({A4, C2, D2, E2, F2, A7});
    pos4 = generatePos({A4, A5, A6, A7, C2, D2, E2, F2, G2});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 4);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 5);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 6);

    square = A3;
    pos1 = generatePos({A4, C3, A2});
    pos2 = generatePos({A4, C3, G3, D3, A5, A6, A2});
    pos3 = generatePos({A4, C3, D3, E3, F3, A7, A2});
    pos4 = generatePos({A2, A4, A5, A6, A7, C3, D3, E3, F3, G3});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 7);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 8);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 9);

    square = A4;
    pos1 = generatePos({A2, A5, C4});
    pos2 = generatePos({A2, A5, A6, C4, F4, G4});
    pos3 = generatePos({A2, A5, A6, C4, D4, F4, G4});
    pos4 = generatePos({A2, A5, A6, C4, D4, E4, F4, G4});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 10);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 11);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 12);

    square = B1;
    pos1 = generatePos({B4, C1});
    pos2 = generatePos({B4, B5, B6, C1, G1, D1});
    pos3 = generatePos({B4, B7, C1, D1, E1, F1});
    pos4 = generatePos({B4, B5, B6, B7, C1, D1, E1, F1, G1});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 13);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 14);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 15);

    square = B2;
    pos1 = generatePos({B4, C2});
    pos2 = generatePos({B4, B5, B6, C2, G2, D2});
    pos3 = generatePos({B4, B7, C2, D2, E2, F2});
    pos4 = generatePos({B4, B5, B6, B7, C2, D2, E2, F2, G2});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 16);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 17);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 18);

    square = B3;
    pos1 = generatePos({B2, B4, C3});
    pos2 = generatePos({B2, B4, B5, B6, C3, G3, D3});
    pos3 = generatePos({B2, B4, B7, C3, D3, E3, F3});
    pos4 = generatePos({B2, B4, B5, B6, B7, C3, D3, E3, F3, G3});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 19);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 20);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 21);

    square = B4;
    pos1 = generatePos({B5, D4});
    pos2 = generatePos({B5, B6, D4, E4});
    pos3 = generatePos({B5, B6, D4, E4, F4});
    pos4 = generatePos({B5, B6, B7, D4, E4, F4, G4});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 22);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 23);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 24);

    square = C1;
    pos1 = generatePos({C4, D1});
    pos2 = generatePos({C4, C5, C6, D1, E1, F1});
    pos3 = generatePos({C4, C7, D1, G1});
    pos4 = generatePos({C4, C5, C6, C7, D1, E1, F1, G1});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 25);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 26);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 27);

    square = C2;
    pos1 = generatePos({C4, D2});
    pos2 = generatePos({C4, C5, C6, D2, E2, F2});
    pos3 = generatePos({C4, C7, D2, G2});
    pos4 = generatePos({C4, C5, C6, C7, D2, E2, F2, G2});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 28);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 29);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 30);

    square = C3;
    pos1 = generatePos({C4, D3});
    pos2 = generatePos({C4, C5, C6, D3, E3, F3});
    pos3 = generatePos({C4, C7, D3, G3});
    pos4 = generatePos({C4, C5, C6, C7, D3, E3, F3, G3});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 31);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 32);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 33);

    square = C4;
    pos1 = generatePos({C2, C5, D4});
    pos2 = generatePos({C2, C5, C6, D4, E4, F4});
    pos3 = generatePos({C2, C5, C7, D4, G4});
    pos4 = generatePos({C2, C5, C6, C7, D4, E4, F4, G4});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 34);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 35);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 36);

    square = D1;
    pos1 = generatePos({D4, B1, F1});
    pos2 = generatePos({D4, D5, B1, F1});
    pos3 = generatePos({D4, D5, B1, F1, G1});
    pos4 = generatePos({D4, D5, D6, D7, B1, F1, G1});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 37);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 38);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 39);

    square = D2;
    pos1 = generatePos({D4, B2, F2});
    pos2 = generatePos({D4, D5, B2, F2});
    pos3 = generatePos({D4, D5, B2, F2, G2});
    pos4 = generatePos({D4, D5, D6, D7, B2, F2, G2});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 40);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 41);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 42);

    square = D3;
    pos1 = generatePos({D2, D4, B3, F3});
    pos2 = generatePos({D2, D4, D5, B3, F3});
    pos3 = generatePos({D2, D4, D5, B3, F3, G3});
    pos4 = generatePos({D2, D4, D5, D6, D7, B3, F3, G3});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 43);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 44);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 45);

    square = D4;
    pos1 = generatePos({D2, D3, D5, B4, F4});
    pos2 = generatePos({D2, D3, D5, B4, F4});
    pos3 = generatePos({D2, D3, D5, B4, F4, G4});
    pos4 = generatePos({D2, D3, D5, D6, D7, B4, F4, G4});
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos1, square), rookReachIndex(pos2, square), 46);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos2, square), rookReachIndex(pos3, square), 47);
    assertEquals<int>(ROOK_REACH_INDEX, rookReachIndex(pos3, square), rookReachIndex(pos4, square), 48);
}

void run_getRookMovesIndex_tests() {
    for (int sq = A1; sq <= H8; sq++) {
        TOTAL_TEST_COUNT++;
        TESTS_COUNTS[ROOK_MOVES_INDEX]++;
        std::string testName = TESTS_NAMES[ROOK_MOVES_INDEX] + std::to_string(TESTS_COUNTS[ROOK_MOVES_INDEX]);

        int northSize = std::max(7 - (sq / 8), 0);
        int southSize = std::max(sq / 8, 0);
        int eastSize = std::max(7 - (sq % 8), 0);
        int westSize = std::max(sq % 8, 0);

        // Build reach
        std::unordered_set<int> indices;
        bool bad = false;
        for (std::array<int, 4> selection : getEndCombinations({northSize, eastSize, southSize, westSize})) {
            uint64_t occ = 0ULL;

            for (int i = 0; i < selection[0]; i++) occ |= 1ULL << (sq + N * (i + 1));
            for (int i = 0; i < selection[1]; i++) occ |= 1ULL << (sq + E * (i + 1));
            for (int i = 0; i < selection[2]; i++) occ |= 1ULL << (sq + S * (i + 1));
            for (int i = 0; i < selection[3]; i++) occ |= 1ULL << (sq + W * (i + 1));

            uint16_t magicIndex = rookMovesIndex(occ, (Square)sq);
            if (indices.find(magicIndex) != indices.end()) {
                bad = true;
                break;
            }
            indices.insert(magicIndex);
        }

        if (bad) {
            TESTS_FAILED++;
            std::cout << testName << "\t[ \033[0;31mFAILED\033[0m ]\tSquare: " << squareName[sq] << '\n';
        } else {
            TESTS_PASSED++;
            std::cout << testName << "\t[ \033[0;32mPASSED\033[0m ]\t" << '\n';
        }
    }
}

void run_getBishopReachIndex_tests() {
    Square square = A1;
    Bitboard pos1 = generatePos({B2});
    Bitboard pos2 = generatePos({B2, C3});
    Bitboard pos3 = generatePos({B2, C3, D4});
    Bitboard pos4 = generatePos({B2, C3, D4, E5, F6, G7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 1);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 2);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 3);

    square = A2;
    pos1 = generatePos({B3});
    pos2 = generatePos({B3, C4});
    pos3 = generatePos({B3, C4, D5});
    pos4 = generatePos({B3, C4, D5, E6, F7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 4);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 5);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 6);

    square = A3;
    pos1 = generatePos({B2, B4});
    pos2 = generatePos({B2, B4, C5});
    pos3 = generatePos({B2, B4, C5, D6});
    pos4 = generatePos({B2, B4, C5, D6, E7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 7);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 8);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 9);

    square = A4;
    pos1 = generatePos({B3, B5});
    pos2 = generatePos({B3, C2, B5, C6});
    pos3 = generatePos({B3, C2, B5, C6, D7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 10);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 11);

    square = B1;
    pos1 = generatePos({C2});
    pos2 = generatePos({C2, D3});
    pos3 = generatePos({C2, D3, E4});
    pos4 = generatePos({C2, D3, E4, F5, G6});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 12);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 13);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 14);

    square = B2;
    pos1 = generatePos({C3});
    pos2 = generatePos({C3, D4});
    pos3 = generatePos({C3, D4, E5});
    pos4 = generatePos({C3, D4, E5, F6, G7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 15);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 16);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 17);

    square = B3;
    pos1 = generatePos({C2, C4});
    pos2 = generatePos({C2, C4, D5});
    pos3 = generatePos({C2, C4, D5, E6});
    pos4 = generatePos({C2, C4, D5, E6, F7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 18);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 19);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 20);

    square = B4;
    pos1 = generatePos({C3, D2, C5});
    pos2 = generatePos({C3, D2, C5, D6});
    pos3 = generatePos({C3, D2, C5, D6, E7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 21);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 22);

    square = C1;
    pos1 = generatePos({B2, D2});
    pos2 = generatePos({B2, D2, E3});
    pos3 = generatePos({B2, D2, E3, F4});
    pos4 = generatePos({B2, D2, E3, F4, G5});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 23);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 24);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 25);

    square = C2;
    pos1 = generatePos({B3, D3});
    pos2 = generatePos({B3, D3, E4});
    pos3 = generatePos({B3, D3, E4, F5});
    pos4 = generatePos({B3, D3, E4, F5, G6});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 26);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 27);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 28);

    square = C3;
    pos1 = generatePos({B2, B4, D2, D4});
    pos2 = generatePos({B2, B4, D2, D4, E5});
    pos3 = generatePos({B2, B4, D2, D4, E5, F6});
    pos4 = generatePos({B2, B4, D2, D4, E5, F6, G7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 29);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 30);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 31);

    square = C4;
    pos1 = generatePos({B3, B5, D3, D5});
    pos2 = generatePos({B3, B5, D3, D5, E2, E6});
    pos3 = generatePos({B3, B5, D3, D5, E2, E6, F7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 32);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 33);

    square = D1;
    pos1 = generatePos({B4, E3});
    pos2 = generatePos({B4, E3, F4});
    pos3 = generatePos({B4, E3, F4, G5});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 34);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 35);

    square = D2;
    pos1 = generatePos({B4, F4});
    pos2 = generatePos({B4, F4, G5});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 36);

    square = D3;
    pos1 = generatePos({C2, E2, C4, E4, B5});
    pos2 = generatePos({C2, E2, C4, E4, F5});
    pos3 = generatePos({C2, E2, C4, E4, B5, F5});
    pos4 = generatePos({C2, E2, C4, E4, B5, F5, G6});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 37);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 38);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 39);

    square = D4;
    pos1 = generatePos({B2, C5, E3, F6});
    pos2 = generatePos({B2, C5, E3, F6, B6});
    pos3 = generatePos({B2, C5, E3, F6, B6, F2});
    pos4 = generatePos({B2, C5, E3, F6, B6, F2, G7});
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos1, square), bishopReachIndex(pos2, square), 40);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos2, square), bishopReachIndex(pos3, square), 41);
    assertEquals<int>(BISHOP_REACH_INDEX, bishopReachIndex(pos3, square), bishopReachIndex(pos4, square), 42);
}

void run_getBishopMovesIndex_tests() {
    for (int sq = A1; sq <= H8; sq++) {
        TOTAL_TEST_COUNT++;
        TESTS_COUNTS[BISHOP_MOVES_INDEX]++;
        std::string testName = TESTS_NAMES[BISHOP_MOVES_INDEX] + std::to_string(TESTS_COUNTS[BISHOP_MOVES_INDEX]);

        int northEastSize = std::max(std::min(7 - sq / 8, 7 - sq % 8), 0);
        int southEastSize = std::max(std::min(sq / 8, 7 - sq % 8), 0);
        int southWestSize = std::max(std::min(sq / 8, sq % 8), 0);
        int northWestSize = std::max(std::min(7 - sq / 8, sq % 8), 0);

        // Build reach
        std::unordered_set<int> indices;
        bool bad = false;
        for (std::array<int, 4> selection : getEndCombinations({northEastSize, southEastSize, southWestSize, northWestSize})) {
            uint64_t occ = 0ULL;

            for (int i = 0; i < selection[0]; i++) occ |= 1ULL << (sq + NE * (i + 1));
            for (int i = 0; i < selection[1]; i++) occ |= 1ULL << (sq + SE * (i + 1));
            for (int i = 0; i < selection[2]; i++) occ |= 1ULL << (sq + SW * (i + 1));
            for (int i = 0; i < selection[3]; i++) occ |= 1ULL << (sq + NW * (i + 1));

            uint16_t magicIndex = bishopMovesIndex(occ, (Square)sq);
            if (indices.find(magicIndex) != indices.end()) {
                bad = true;
                break;
            }
            indices.insert(magicIndex);
        }

        if (bad) {
            TESTS_FAILED++;
            std::cout << testName << "\t[ \033[0;31mFAILED\033[0m ]\tSquare: " << squareName[sq] << '\n';
        } else {
            TESTS_PASSED++;
            std::cout << testName << "\t[ \033[0;32mPASSED\033[0m ]\t" << '\n';
        }
    }
}

void run_knightMagicNums_tests() {
    for (int sq = 0; sq < 64; sq++) {
        TOTAL_TEST_COUNT++;
        TESTS_COUNTS[KNIGHT_MAGIC_NUMS]++;
        std::string testName = TESTS_NAMES[KNIGHT_MAGIC_NUMS] + std::to_string(TESTS_COUNTS[KNIGHT_MAGIC_NUMS]);

        std::vector<int> destinations;
        if (sq / 8 < 6 && sq % 8 < 7) destinations.push_back(sq + NNE);
        if (sq / 8 < 7 && sq % 8 < 6) destinations.push_back(sq + ENE);
        if (sq / 8 > 0 && sq % 8 < 6) destinations.push_back(sq + ESE);
        if (sq / 8 > 1 && sq % 8 < 7) destinations.push_back(sq + SES);
        if (sq / 8 > 1 && sq % 8 > 0) destinations.push_back(sq + SWS);
        if (sq / 8 > 0 && sq % 8 > 1) destinations.push_back(sq + WSW);
        if (sq / 8 < 7 && sq % 8 > 1) destinations.push_back(sq + WNW);
        if (sq / 8 < 6 && sq % 8 > 0) destinations.push_back(sq + NWN);

        uint16_t maxOccupancy = (uint16_t)pow(2, destinations.size());
        bool duplicate = false;
        std::unordered_set<int> indices;

        // Build occupancy bitboard
        for (uint16_t j = 0; j < maxOccupancy; j++) {
            uint64_t occ = 0ULL;
            int shift = 0;
            for (int dest : destinations) {
                occ |= ((j >> shift) & 1UL) << dest;
                shift++;
            }

            uint16_t magicIndex = getKnightMovesIndex(occ, (Square) sq);
            if (indices.find(magicIndex) != indices.end()) {
                duplicate = true;
                break;
            }
            indices.insert(magicIndex);
        }

        if (duplicate) {
            TESTS_FAILED++;
            std::cout << testName << "\t[ \033[0;31mFAILED\033[0m ]\tSquare: " << squareName[sq] << '\n';
        } else {
            TESTS_PASSED++;
            std::cout << testName << "\t[ \033[0;32mPASSED\033[0m ]\t" << '\n';
        }
    }
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
    run_knightMagicNums_tests();
    printFinalResult();
    return 0;
}