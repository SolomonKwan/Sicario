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

using namespace std;

// Useful things for later
// cout << bitset<64>(_pext_u64(0b100101ULL, 0b101101ULL)) << '\n';
// cout << _popcnt64(9ULL) << '\n';
// cout << _popcnt64(0ULL) << '\n';

enum Direction {
    NORTH =  8,
    NORTHEAST = 9,
    EAST = 1,
    SOUTHEAST = -7,
    SOUTH = -8,
    SOUTHWEST = -9,
    WEST = -1,
    NORTHWEST = 7
};

const std::string squareName[65] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "None"
};

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
        1, 2, 3, 4, 5, 6, 7, 8,
        9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56,
        57, 58, 59, 60, 61, 62, 63, 64
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

void findRookMNs() {
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
                for (int k = 0; k < northSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + NORTH * (k + 1));
                for (int k = 0; k < southSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + SOUTH * (k + 1));
                for (int k = 0; k < eastSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + EAST * (k + 1));
                for (int k = 0; k < westSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + WEST * (k + 1));

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

void findBishopMNs() {
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
                for (int k = 0; k < northEastSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + NORTHEAST * (k + 1));
                for (int k = 0; k < southEastSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + SOUTHEAST * (k + 1));
                for (int k = 0; k < southWestSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + SOUTHWEST * (k + 1));
                for (int k = 0; k < northWestSize; k++, shift++) occ |= ((j >> shift) & 1UL) << (sq + NORTHWEST * (k + 1));

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

int main() {
    cout << "Rook magic numbers" << '\n';
    findRookMNs();
    cout << "Bishop magic numbers" << '\n';
    findBishopMNs();
}