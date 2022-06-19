#include <iostream>
#include <bitset>
#include <random>
#include <map>
#include <iomanip>
#include <algorithm>
#include <x86intrin.h>
#include <cmath>
#include <unordered_set>
#include <sstream>
#include <vector>
#include "../src/constants.hpp"
#include "../src/movegen.hpp"

using namespace std;

// This file is just for messing around and testing stuff. no need to keep it clean or anything.

// Useful things for later
// cout << bitset<64>(_pext_u64(0b100101ULL, 0b101101ULL)) << '\n';
// cout << _popcnt64(9ULL) << '\n';
// cout << _popcnt64(0ULL) << '\n';

void displayBB(uint64_t position) {
    string positionString = bitset<64>(position).to_string();
    for (int i = 0; i < 8; i++) {
        // Reverse each line then print.
        string line = positionString.substr(8 * i, 8);
        reverse(line.begin(), line.end());
        cout << line << '\n';
    }
    cout << '\n' << flush;
}

uint64_t randomMagicNumber(float dist) {
    array<int, 64> squares = {
        0, 1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56,
        57, 58, 59, 60, 61, 62, 63
    };

    random_device rd;
    mt19937 g(rd());
    shuffle(squares.begin(), squares.end(), g);

    uint64_t res = 0ULL;
    for (int i = 0; i < (int)(dist * 64); i++) {
        res |= 1ULL << squares[i];
    }

    return res;
}

void findRookReachMNs() {
    for (int sq = 0; sq < 64; sq++) {
        int northSize = max(6 - (sq / 8), 0);
        int southSize = max(sq / 8 - 1, 0);
        int eastSize = max(6 - (sq % 8), 0);
        int westSize = max(sq % 8 - 1, 0);
        int totalSize = northSize + southSize + eastSize + westSize;

        uint64_t maxOccupancy = pow(2, totalSize);
        uint16_t indexMask = 0;
        uint64_t magicNum;
        for (int i = 0; i < totalSize; i++) indexMask |= ((uint16_t)1 << i);

        int attempt = 0;
        float prob = 0.05;
        while (true) {
            if (attempt > 100000) {
                attempt = 0;
                prob += 0.05;
            }

            if (prob >= 1) {
                cout << "Welp, the square " << squareName[sq] << " was fucked..." << '\n';
                break;
            }
            magicNum = randomMagicNumber(prob);
            unordered_set<int> indices;
            bool duplicate = false;

            // Build occupancy bitboard
            for (uint64_t j = 0; j < maxOccupancy; j++) {
                uint64_t occ = 0ULL;
                int shift = 0;
                for (int k = 0; k < northSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + N * (k + 1));
                for (int k = 0; k < southSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + S * (k + 1));
                for (int k = 0; k < eastSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + E * (k + 1));
                for (int k = 0; k < westSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + W * (k + 1));

                uint16_t magicIndex = ((occ * magicNum) >> (64 - totalSize)) & indexMask;
                if (indices.find(magicIndex) != indices.end()) {
                    duplicate = true;
                    break;
                }

                indices.insert(magicIndex);
            }

            if (!duplicate) {
                cout << squareName[sq] << " 0x" << std::hex << magicNum << " \t" << std::dec << (64 - totalSize) << '\n';
                break;
            }
            attempt++;
        }
    }
}

void findRookMovesMNs() {
    for (int sq = 0; sq < 64; sq++) {
        if (sq == A1 || sq == F1 || sq == H1 || sq == H3 || sq == A6 || sq == H6 || sq == H7 || sq == A8 || sq == F8 || sq == H8 || sq == C1
                || sq == C1 || sq == G7 || sq == C8 || sq == E2 || sq == B8 || sq == E8 || sq == G8 || sq == H4 || sq == H5 || sq == B1
                || sq == D1 || sq == E1 || sq == G1 || sq == A2 || sq == B2 || sq == D2 || sq == G2 || sq == H2 || sq == A3 || sq == C3
                || sq == F3 || sq == A4 || sq == B4 || sq == G4 || sq == A5 || sq == B5 || sq == G5 || sq == F6 || sq == A7 || sq == B7
                || sq == D7 || sq == E7 || sq == D8 || sq == F4 || sq == F5 || sq == C7 || sq == F7 || sq == C6 || sq == F2 || sq == E5
                || sq == G6 || sq == B6 || sq == E3 || sq == C5 || sq == D6) continue;
        int northSize = max(7 - (sq / 8), 0);
        int southSize = max(sq / 8, 0);
        int eastSize = max(7 - (sq % 8), 0);
        int westSize = max(sq % 8, 0);
        int totalSize = (northSize ? northSize + 1 : 1) * (eastSize ? eastSize + 1 : 1) *
                (southSize ? southSize + 1 : 1) * (westSize ? westSize + 1 : 1);
        totalSize = (int)floor(log2(totalSize)) + 4;
        auto combos = getEndCombinations({northSize, eastSize, southSize, westSize});

        int attempt = 0;
        float prob = 0.5;
        while (true) {
            if (attempt > 100000) {
                attempt = 0;
                prob += 0.05;
            }

            if (prob > 1) {
                cout << "Welp, the square " << squareName[sq] << " was fucked..." << '\n';
                break;
            }
            uint64_t magicNum = randomMagicNumber(prob);

            unordered_set<int> indices;
            bool duplicate = false;

            // Build occupancy bitboard
            for (std::array<int, 4> selection : combos) {
                uint64_t occ = 0ULL;

                for (int i = 0; i < selection[0]; i++) occ |= 1ULL << (sq + N * (i + 1));
                for (int i = 0; i < selection[1]; i++) occ |= 1ULL << (sq + E * (i + 1));
                for (int i = 0; i < selection[2]; i++) occ |= 1ULL << (sq + S * (i + 1));
                for (int i = 0; i < selection[3]; i++) occ |= 1ULL << (sq + W * (i + 1));

                uint16_t magicIndex = ((occ * magicNum) >> (64 - totalSize));
                if (indices.find(magicIndex) != indices.end()) {
                    duplicate = true;
                    break;
                }

                indices.insert(magicIndex);
            }

            if (!duplicate) {
                cout << squareName[sq] << " 0x" << std::hex << magicNum << " \t" << std::dec << (64 - totalSize) << '\n';
                break;
            }
            attempt++;
        }
    }
}

void findBishopReachMNs() {
    for (int sq = 0; sq < 64; sq++) {
        int northEastSize = max(min(6 - sq / 8, 6 - sq % 8), 0);
        int southEastSize = max(min(sq / 8 - 1, 6 - sq % 8), 0);
        int southWestSize = max(min(sq / 8 - 1, sq % 8 - 1), 0);
        int northWestSize = max(min(6 - sq / 8, sq % 8 - 1), 0);
        int totalSize = northEastSize + southEastSize + southWestSize + northWestSize;

        uint64_t maxOccupancy = pow(2, totalSize);
        uint16_t indexMask = 0;
        uint64_t magicNum;
        for (int i = 0; i < totalSize; i++) indexMask |= ((uint16_t)1 << i);

        int attempt = 0;
        float prob = 0.05;
        while (true) {
            if (attempt > 100000) {
                attempt = 0;
                prob += 0.05;
            }

            if (prob >= 1) {
                cout << "Welp, the square " << squareName[sq] << " was fucked..." << '\n';
                break;
            }
            magicNum = randomMagicNumber(prob);
            unordered_set<int> indices;
            bool duplicate = false;

            // Build occupancy bitboard
            for (uint64_t j = 0; j < maxOccupancy; j++) {
                uint64_t occ = 0ULL;
                int shift = 0;
                for (int k = 0; k < northEastSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + NE * (k + 1));
                for (int k = 0; k < southEastSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + SE * (k + 1));
                for (int k = 0; k < southWestSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + SW * (k + 1));
                for (int k = 0; k < northWestSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + NW * (k + 1));

                uint16_t magicIndex = ((occ * magicNum) >> (64 - totalSize)) & indexMask;
                if (indices.find(magicIndex) != indices.end()) {
                    duplicate = true;
                    break;
                }

                indices.insert(magicIndex);
            }

            if (!duplicate) {
                cout << squareName[sq] << " 0x" << std::hex << magicNum << " \t" << std::dec << (64 - totalSize) << '\n';
                break;
            }
            attempt++;
        }
    }
}

void generateRookMasks() {

}

void generateBishopMasks() {

}

void generateKnightMask() {

}

int main(int argc, char* argv[]) {
    // cout << "Rook magic numbers" << '\n';
    // findRookReachMNs();
    // cout << "Bishop magic numbers" << '\n';
    // findBishopReachMNs();
    findRookMovesMNs();
}