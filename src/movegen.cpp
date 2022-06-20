
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "movegen.hpp"
#include "game.hpp"
#include "utils.hpp"

std::array<std::vector<std::vector<Move>>, 64> computeKingMoves() {
    std::array<std::vector<std::vector<Move>>, 64> kingMoves;
    for (int square = A1; square <= H8; square++) {
        std::vector<std::vector<Move>>& moveSet = kingMoves[square];
        std::vector<Square> destinations;
        if (square / 8 != 7) destinations.push_back((Square) (square + N));
        if (square / 8 != 7 && square % 8 != 7) destinations.push_back((Square) (square + NE));
        if (square % 8 != 7) destinations.push_back((Square) (square + E));
        if (square / 8 != 0 && square % 8 != 7) destinations.push_back((Square) (square + SE));
        if (square / 8 != 0) destinations.push_back((Square) (square + S));
        if (square / 8 != 0 && square % 8 != 0) destinations.push_back((Square) (square + SW));
        if (square % 8 != 0) destinations.push_back((Square) (square + W));
        if (square / 8 != 7 && square % 8 != 0) destinations.push_back((Square) (square + NW));

        uint16_t maxOccupancy = (uint16_t)pow(2, destinations.size());
        moveSet.resize(maxOccupancy);

        // Build occupancy bitboard
        for (uint16_t j = 0; j < maxOccupancy; j++) {
            std::vector<Move> moves;

            uint64_t occ = 0ULL;
            int shift = 0;
            for (Square dest : destinations) {
                if ((j >> shift) & 1UL) {
                    moves.push_back(square | (dest << 6));
                }
                occ |= ((j >> shift) & 1UL) << dest;
                shift++;
            }

            uint16_t magicIndex = getKingMovesIndex(occ, (Square) square);
            moveSet[magicIndex] = moves;
        }
    }
    return kingMoves;
}

std::array<std::vector<std::vector<Move>>, 64> computeKnightMoves() {
    std::array<std::vector<std::vector<Move>>, 64> knightMoves;
    for (int square = A1; square <= H8; square++) {
        std::vector<std::vector<Move>>& moveSet = knightMoves[square];
        std::vector<Square> destinations;
        if (square / 8 < 6 && square % 8 < 7) destinations.push_back((Square) (square + NNE));
        if (square / 8 < 7 && square % 8 < 6) destinations.push_back((Square) (square + ENE));
        if (square / 8 > 0 && square % 8 < 6) destinations.push_back((Square) (square + ESE));
        if (square / 8 > 1 && square % 8 < 7) destinations.push_back((Square) (square + SES));
        if (square / 8 > 1 && square % 8 > 0) destinations.push_back((Square) (square + SWS));
        if (square / 8 > 0 && square % 8 > 1) destinations.push_back((Square) (square + WSW));
        if (square / 8 < 7 && square % 8 > 1) destinations.push_back((Square) (square + WNW));
        if (square / 8 < 6 && square % 8 > 0) destinations.push_back((Square) (square + NWN));

        uint16_t maxOccupancy = (uint16_t)pow(2, destinations.size());
        moveSet.resize(maxOccupancy);

        // Build occupancy bitboard
        for (uint16_t j = 0; j < maxOccupancy; j++) {
            std::vector<Move> moves;

            uint64_t occ = 0ULL;
            int shift = 0;
            for (Square dest : destinations) {
                if ((j >> shift) & 1UL) {
                    moves.push_back(square | (dest << 6));
                }
                occ |= ((j >> shift) & 1UL) << dest;
                shift++;
            }

            uint16_t magicIndex = getKnightMovesIndex(occ, (Square) square);
            moveSet[magicIndex] = moves;
        }
    }
    return knightMoves;
}

std::array<std::array<std::vector<std::vector<Move>>, 64>, 2> computePawnMoves() {
    std::array<std::array<std::vector<std::vector<Move>>, 64>, 2> pawnMoves = {
        computePawnMovesBySide(BLACK),
        computePawnMovesBySide(WHITE)
    };
    return pawnMoves;
}

std::array<std::vector<std::vector<Move>>, 64> computePawnMovesBySide(Player player) {
    std::array<std::vector<std::vector<Move>>, 64> pawnMoves;
    for (int square = A2; square <= H7; square++) {
        std::vector<std::vector<Move>>& moveSet = pawnMoves[square];
        std::vector<int> destinations;
        if (player == BLACK) {
            destinations.push_back(square + S);
            if (square / 8 == 6) destinations.push_back(square + S + S);
            if (square % 8 != 0) destinations.push_back(square + SW);
            if (square % 8 != 7) destinations.push_back(square + SE);
        } else {
            destinations.push_back(square + N);
            if (square / 8 == 1) destinations.push_back(square + N + N);
            if (square % 8 != 0) destinations.push_back(square + NW);
            if (square % 8 != 7) destinations.push_back(square + NE);
        }

        int totalSize = destinations.size();
        uint16_t maxOccupancy = (uint16_t)pow(2, totalSize);
        moveSet.resize(maxOccupancy);

        for (uint16_t j = 0; j < maxOccupancy; j++) {
            std::vector<Move> moves;

            uint64_t occ = 0ULL;
            int shift = 0;
            for (int dest : destinations) {
                if ((j >> shift) & 1UL) {
                    if (dest / 8 == 0 || dest / 8 == 7) {
                        moves.push_back(square | (dest << 6) | PROMOTION | pQUEEN);
                        moves.push_back(square | (dest << 6) | PROMOTION | pROOK);
                        moves.push_back(square | (dest << 6) | PROMOTION | pBISHOP);
                        moves.push_back(square | (dest << 6) | PROMOTION | pKNIGHT);
                    } else {
                        moves.push_back(square | (dest << 6));
                    }
                }
                occ |= ((j >> shift) & 1UL) << dest;
                shift++;
            }

            uint16_t magicIndex = getPawnMovesIndex(occ, (Square) square, player);
            moveSet[magicIndex] = moves;
        }
    }
    return pawnMoves;
}

std::array<std::vector<std::vector<Move>>, 64> computeRookMoves() {
    std::array<std::vector<std::vector<Move>>, 64> rookMoves;
    for (int square = A1; square <= H8; square++) {
        int northSize = std::max(7 - (square / 8), 0);
        int southSize = std::max(square / 8, 0);
        int eastSize = std::max(7 - (square % 8), 0);
        int westSize = std::max(square % 8, 0);
        std::vector<std::vector<Move>>& movesSet = rookMoves[square];
        movesSet.resize((int)std::pow(2, 64 - rookMovesShifts[square]));

        for (std::array<int, 4> selection : getEndCombinations({northSize, eastSize, southSize, westSize})) {
            uint64_t reach = 0ULL;
            std::vector<Move> moves;

            for (int i = 0; i < selection[0]; i++) {
                reach |= 1ULL << (square + N * (i + 1));
                moves.push_back(square | ((square + N * (i + 1)) << 6));
            }

            for (int i = 0; i < selection[1]; i++) {
                reach |= 1ULL << (square + E * (i + 1));
                moves.push_back(square | ((square + E * (i + 1)) << 6));
            }

            for (int i = 0; i < selection[2]; i++) {
                reach |= 1ULL << (square + S * (i + 1));
                moves.push_back(square | ((square + S * (i + 1)) << 6));
            }

            for (int i = 0; i < selection[3]; i++) {
                reach |= 1ULL << (square + W * (i + 1));
                moves.push_back(square | ((square + W * (i + 1)) << 6));
            }

            int magicIndex = getRookMovesIndex(reach, (Square) square);
            movesSet[magicIndex] = moves;
        }
    }
    return rookMoves;
}

std::array<std::vector<std::vector<Move>>, 64> computeBishopMoves() {
    std::array<std::vector<std::vector<Move>>, 64> bishopMoves;
    for (int square = A1; square <= H8; square++) {
        int northEastSize = std::max(std::min(7 - square / 8, 7 - square % 8), 0);
        int southEastSize = std::max(std::min(square / 8, 7 - square % 8), 0);
        int southWestSize = std::max(std::min(square / 8, square % 8), 0);
        int northWestSize = std::max(std::min(7 - square / 8, square % 8), 0);
        std::vector<std::vector<Move>>& movesSet = bishopMoves[square];
        movesSet.resize((int)std::pow(2, 64 - bishopMovesShifts[square]));

        for (std::array<int, 4> selection : getEndCombinations({northEastSize, southEastSize, southWestSize,
                northWestSize})) {
            uint64_t reach = 0ULL;
            std::vector<Move> moves;

            for (int i = 0; i < selection[0]; i++) {
                reach |= 1ULL << (square + NE * (i + 1));
                moves.push_back(square | ((square + NE * (i + 1)) << 6));
            }

            for (int i = 0; i < selection[1]; i++) {
                reach |= 1ULL << (square + SE * (i + 1));
                moves.push_back(square | ((square + SE * (i + 1)) << 6));
            }

            for (int i = 0; i < selection[2]; i++) {
                reach |= 1ULL << (square + SW * (i + 1));
                moves.push_back(square | ((square + SW * (i + 1)) << 6));
            }

            for (int i = 0; i < selection[3]; i++) {
                reach |= 1ULL << (square + NW * (i + 1));
                moves.push_back(square | ((square + NW * (i + 1)) << 6));
            }

            int magicIndex = getBishopMovesIndex(reach, (Square) square);
            movesSet[magicIndex] = moves;
        }
    }
    return bishopMoves;
}

std::array<std::vector<int>, 64> computeRookReachIndices() {
    std::array<std::vector<int>, 64> rookIndices;
    for (int square = A1; square <= H8; square++) {
        std::vector<int>& indices = rookIndices[square];
        int northSize = std::max(6 - (square / 8), 0);
        int southSize = std::max(square / 8 - 1, 0);
        int eastSize = std::max(6 - (square % 8), 0);
        int westSize = std::max(square % 8 - 1, 0);
        int totalSize = northSize + southSize + eastSize + westSize;
        indices.resize(pow(2, totalSize));

        uint16_t maxOccupancy = pow(2, totalSize);
        for (uint16_t j = 0; j < maxOccupancy; j++) {
            int northBlock = 0, eastBlock = 0, southBlock = 0, westBlock = 0;
            uint64_t occ = 0ULL;
            int shift = 0;
            for (int k = 0; k < northSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + N * (k + 1));
                if (!northBlock && ((j >> shift) & 1UL)) northBlock = k + 1;
            }
            for (int k = 0; k < eastSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + E * (k + 1));
                if (!eastBlock && ((j >> shift) & 1UL)) eastBlock = k + 1;
            }
            for (int k = 0; k < southSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + S * (k + 1));
                if (!southBlock && ((j >> shift) & 1UL)) southBlock = k + 1;
            }
            for (int k = 0; k < westSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + W * (k + 1));
                if (!westBlock && ((j >> shift) & 1UL)) westBlock = k + 1;
            }

            uint16_t magicIndex = ((occ * rookReachMagicNumbers[square]) >> rookReachShifts[square]);
            int mappedIndex = getIndex(
                {northBlock, eastBlock, southBlock, westBlock},
                {northSize ? northSize + 1 : 0, eastSize ? eastSize + 1 : 0, southSize ? southSize + 1 : 0,
                        westSize ? westSize + 1 : 0}
            );
            indices[magicIndex] = mappedIndex;
        }
    }
    return rookIndices;
}

std::array<std::vector<int>, 64> computeBishopReachIndices() {
    std::array<std::vector<int>, 64> bishopIndices;
    for (int square = A1; square <= H8; square++) {
        std::vector<int>& indices = bishopIndices[square];
        int northeastSize = std::max(std::min(6 - square / 8, 6 - square % 8), 0);
        int southeastSize = std::max(std::min(square / 8 - 1, 6 - square % 8), 0);
        int southwestSize = std::max(std::min(square / 8 - 1, square % 8 - 1), 0);
        int northwestSize = std::max(std::min(6 - square / 8, square % 8 - 1), 0);
        int totalSize = northeastSize + southeastSize + southwestSize + northwestSize;
        indices.resize(pow(2, totalSize));

        uint16_t maxOccupancy = pow(2, totalSize);
        for (uint16_t j = 0; j < maxOccupancy; j++) {
            int northeastBlock = 0, southeastBlock = 0, southwestBlock = 0, northwestBlock = 0;
            uint64_t occ = 0ULL;
            int shift = 0;
            for (int k = 0; k < northeastSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + NE * (k + 1));
                if (!northeastBlock && ((j >> shift) & 1UL)) northeastBlock = k + 1;
            }
            for (int k = 0; k < southeastSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + SE * (k + 1));
                if (!southeastBlock && ((j >> shift) & 1UL)) southeastBlock = k + 1;
            }
            for (int k = 0; k < southwestSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + SW * (k + 1));
                if (!southwestBlock && ((j >> shift) & 1UL)) southwestBlock = k + 1;
            }
            for (int k = 0; k < northwestSize; k++, shift++) {
                occ |= ((j >> shift) & 1UL) << (square + NW * (k + 1));
                if (!northwestBlock && ((j >> shift) & 1UL)) northwestBlock = k + 1;
            }

            uint16_t magicIndex = ((occ * bishopReachMagicNumbers[square]) >> bishopReachShifts[square]);
            int mappedIndex = getIndex(
                {northeastBlock, southeastBlock, southwestBlock, northwestBlock},
                {northeastSize ? northeastSize + 1 : 0, southeastSize ? southeastSize + 1 : 0,
                        southwestSize ? southwestSize + 1 : 0, northwestSize ? northwestSize + 1 : 0}
            );
            indices[magicIndex] = mappedIndex;
        }
    }
    return bishopIndices;
}

int getIndex(std::vector<int> values, std::vector<int> ranges) {
    if (values.size() == 0) return values[0];

    // Iteratively build the index (basically just a composition of functions i.e. f(f(f(...)))).
    int currValue = -1, currRange = -1;
    for (int i = 0; i < (int)ranges.size(); i++) {
        if (ranges[i] != 0 && currValue == -1) {
            currValue = values[i];
            currRange = ranges[i];
        } else if (ranges[i] != 0) {
            currValue = pairingFunction(currValue, values[i], currRange, ranges[i]);
            currRange =  currRange * ranges[i];
        }
    }

    return currValue;
}

int pairingFunction(int n, int m, int N, int M) {
    if (N == M) {
        return n + m * N;
    } else if (N < M) {
        return m + n * M;
    } else {
        return n + m * N;
    }
}

void generateCombination(std::array<int, 4> sizes, std::array<int, 4>& curr, std::vector<std::array<int, 4>>& res) {
    res.push_back(curr);
    if (curr == sizes) return;

    // Increment first number
    curr[0]++;

    // If there is an overflow, carry over the overflow.
    for (int i = 0; i < (int)curr.size(); i++) {
        if (curr[i] > sizes[i]) {
            curr[i] = 0;
            if (i != (int)curr.size() - 1) curr[i + 1]++;
        }
    }

    generateCombination(sizes, curr, res);
}

std::vector<std::array<int, 4>> getEndCombinations(std::array<int, 4> sizes) {
    std::vector<std::array<int, 4>> res;
    std::array<int, 4> curr = {0, 0, 0, 0};
    generateCombination(sizes, curr, res);
    return res;
}