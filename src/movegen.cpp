
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
        movesSet.resize((int)std::pow(2, 64 - Shifts::Moves::ROOK[square]));

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
        movesSet.resize((int)std::pow(2, 64 - Shifts::Moves::BISHOP[square]));

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

std::array<std::vector<Move>, 4> computeCastlingMoves() {
    std::array<std::vector<Move>, 4> moves;
    moves[WKSC] = {E1 | G1 << 6 | CASTLING};
    moves[WQSC] = {E1 | C1 << 6 | CASTLING};
    moves[BKSC] = {E8 | G8 << 6 | CASTLING};
    moves[BQSC] = {E8 | C8 << 6 | CASTLING};
    return moves;
}

std::array<std::vector<std::vector<Move>>, 64> computeRookBlockMoves() {
    std::array<std::vector<std::vector<Move>>, 64> rookBlockMoves;
    for (int square = A1; square <= H8; square++) {
        int northSize = std::max(7 - (square / 8), 0);
        int southSize = std::max(square / 8, 0);
        int eastSize = std::max(7 - (square % 8), 0);
        int westSize = std::max(square % 8, 0);
        std::vector<std::vector<Move>>& movesSet = rookBlockMoves[square];
        movesSet.resize((int)std::pow(2, 64 - Shifts::Block::ROOK[square]));

        for (std::array<int, 4> selection : getEndBlockSquares({northSize, eastSize, southSize, westSize})) {
            std::vector<Move> moves;
            uint64_t occ = 0ULL;
            if (selection[0]) {
                moves.push_back(square | (square + (N * selection[0])) << 6);
                occ |= 1ULL << (square + (N * selection[0]));
            }

            if (selection[1]) {
                moves.push_back(square | (square + (E * selection[1])) << 6);
                occ |= 1ULL << (square + (E * selection[1]));
            }

            if (selection[2]) {
                moves.push_back(square | (square + (S * selection[2])) << 6);
                occ |= 1ULL << (square + (S * selection[2]));
            }

            if (selection[3]) {
                moves.push_back(square | (square + (W * selection[3])) << 6);
                occ |= 1ULL << (square + (W * selection[3]));
            }

            movesSet[getRookBlockIndex(occ, (Square) square)] = moves;
        }
    }
    return rookBlockMoves;
}

std::array<std::vector<std::vector<Move>>, 64> computeBishopBlockMoves() {
    std::array<std::vector<std::vector<Move>>, 64> bishopBlockMoves;
    for (int square = A1; square <= H8; square++) {
        int northEastSize = std::max(std::min(7 - square / 8, 7 - square % 8), 0);
        int southEastSize = std::max(std::min(square / 8, 7 - square % 8), 0);
        int southWestSize = std::max(std::min(square / 8, square % 8), 0);
        int northWestSize = std::max(std::min(7 - square / 8, square % 8), 0);
        std::vector<std::vector<Move>>& movesSet = bishopBlockMoves[square];
        movesSet.resize((int)std::pow(2, 64 - Shifts::Block::BISHOP[square]));

        for (std::array<int, 4> selection : getEndBlockSquares({northEastSize, southEastSize, southWestSize,
                northWestSize})) {
            std::vector<Move> moves;
            uint64_t occ = 0ULL;
            if (selection[0]) {
                moves.push_back(square | (square + (NE * selection[0])) << 6);
                occ |= 1ULL << (square + (NE * selection[0]));
            }

            if (selection[1]) {
                moves.push_back(square | (square + (SE * selection[1])) << 6);
                occ |= 1ULL << (square + (SE * selection[1]));
            }

            if (selection[2]) {
                moves.push_back(square | (square + (SW * selection[2])) << 6);
                occ |= 1ULL << (square + (SW * selection[2]));
            }

            if (selection[3]) {
                moves.push_back(square | (square + (NW * selection[3])) << 6);
                occ |= 1ULL << (square + (NW * selection[3]));
            }

            movesSet[getBishopBlockIndex(occ, (Square) square)] = moves;
        }
    }
    return bishopBlockMoves;
}

std::array<std::vector<uint>, 64> computeRookReachIndices() {
    std::array<std::vector<uint>, 64> rookIndices;
    for (int square = A1; square <= H8; square++) {
        std::vector<uint>& indices = rookIndices[square];
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

            uint16_t magicIndex = ((occ * MagicNums::Reach::ROOK[square]) >> Shifts::Reach::ROOK[square]);
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

std::array<std::vector<uint>, 64> computeBishopReachIndices() {
    std::array<std::vector<uint>, 64> bishopIndices;
    for (int square = A1; square <= H8; square++) {
        std::vector<uint>& indices = bishopIndices[square];
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

            uint16_t magicIndex = ((occ * MagicNums::Reach::BISHOP[square]) >> Shifts::Reach::BISHOP[square]);
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

std::array<std::vector<Bitboard>, 64> computeRookReaches() {
    const std::array<std::vector<uint>, 64> ROOK = computeRookReachIndices();
    std::array<std::vector<Bitboard>, 64> reaches;
    for (int square = A1; square <= H8; square++) {
        std::vector<Bitboard>& reachSet = reaches[square];
        int northSize = std::max(6 - (square / 8), 0);
        int southSize = std::max(square / 8 - 1, 0);
        int eastSize = std::max(6 - (square % 8), 0);
        int westSize = std::max(square % 8 - 1, 0);
        reachSet.resize(*std::max_element(ROOK[square].begin(), ROOK[square].end()) + 1);
        std::vector<std::array<int, 4>> combos = getEndCombinations({northSize, eastSize, southSize, westSize});

        for (std::array<int, 4> combo : combos) {
            Bitboard occ = 0ULL;
            if (combo[0]) occ |= (1ULL << (square + N * combo[0]));
            if (combo[1]) occ |= (1ULL << (square + E * combo[1]));
            if (combo[2]) occ |= (1ULL << (square + S * combo[2]));
            if (combo[3]) occ |= (1ULL << (square + W * combo[3]));

            Bitboard reach = 0ULL;
            if (northSize && !combo[0]) combo[0] = northSize + 1;
            for (int i = 1; combo[0] > 0; combo[0]--, i++) {
                reach |= (1ULL << (square + i * N));
            }

            if (eastSize && !combo[1]) combo[1] = eastSize + 1;
            for (int i = 1; combo[1] > 0; combo[1]--, i++) {
                reach |= (1ULL << (square + i * E));
            }

            if (southSize && !combo[2]) combo[2] = southSize + 1;
            for (int i = 1; combo[2] > 0; combo[2]--, i++) {
                reach |= (1ULL << (square + i * S));
            }

            if (westSize && !combo[3]) combo[3] = westSize + 1;
            for (int i = 1; combo[3] > 0; combo[3]--, i++) {
                reach |= (1ULL << (square + i * W));
            }

            reachSet[ROOK[square][getRookReachIndex(occ, (Square) square)]] = reach;
        }
    }
    return reaches;
}

std::array<std::vector<Bitboard>, 64> computeBishopReaches() {
    const std::array<std::vector<uint>, 64> BISHOP = computeBishopReachIndices();
    std::array<std::vector<Bitboard>, 64> reaches;
    for (int square = A1; square <= H8; square++) {
        std::vector<Bitboard>& reachSet = reaches[square];
        int northeastSize = std::max(std::min(6 - square / 8, 6 - square % 8), 0);
        int southeastSize = std::max(std::min(square / 8 - 1, 6 - square % 8), 0);
        int southwestSize = std::max(std::min(square / 8 - 1, square % 8 - 1), 0);
        int northwestSize = std::max(std::min(6 - square / 8, square % 8 - 1), 0);
        reachSet.resize(*std::max_element(BISHOP[square].begin(), BISHOP[square].end()) + 1);
        std::vector<std::array<int, 4>> combos = getEndCombinations({northeastSize, southeastSize, southwestSize,
                northwestSize});

        for (std::array<int, 4> combo : combos) {
            Bitboard occ = 0ULL;
            if (combo[0]) occ |= (1ULL << (square + NE * combo[0]));
            if (combo[1]) occ |= (1ULL << (square + SE * combo[1]));
            if (combo[2]) occ |= (1ULL << (square + SW * combo[2]));
            if (combo[3]) occ |= (1ULL << (square + NW * combo[3]));

            Bitboard reach = 0ULL;
            if (northeastSize && !combo[0]) combo[0] = northeastSize + 1;
            for (int i = 1; combo[0] > 0; combo[0]--, i++) {
                reach |= (1ULL << (square + i * NE));
            }

            if (southeastSize && !combo[1]) combo[1] = southeastSize + 1;
            for (int i = 1; combo[1] > 0; combo[1]--, i++) {
                reach |= (1ULL << (square + i * SE));
            }

            if (southwestSize && !combo[2]) combo[2] = southwestSize + 1;
            for (int i = 1; combo[2] > 0; combo[2]--, i++) {
                reach |= (1ULL << (square + i * SW));
            }

            if (northwestSize && !combo[3]) combo[3] = northwestSize + 1;
            for (int i = 1; combo[3] > 0; combo[3]--, i++) {
                reach |= (1ULL << (square + i * NW));
            }

            reachSet[BISHOP[square][getBishopReachIndex(occ, (Square) square)]] = reach;
        }
    }
    return reaches;
}

std::array<std::vector<std::vector<Square>>, 64> computeKingReachSquares() {
    std::array<std::vector<std::vector<Square>>, 64> kingReaches;
    for (int square = A1; square <= H8; square++) {
        std::vector<std::vector<Square>>& reachSet = kingReaches[square];
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
        reachSet.resize(maxOccupancy);

        // Build occupancy bitboard
        for (uint16_t j = 0; j < maxOccupancy; j++) {
            std::vector<Square> reach;

            uint64_t occ = 0ULL;
            int shift = 0;
            for (Square dest : destinations) {
                if ((j >> shift) & 1UL) {
                    reach.push_back(dest);
                }
                occ |= ((j >> shift) & 1UL) << dest;
                shift++;
            }

            uint16_t magicIndex = getKingMovesIndex(occ, (Square) square);
            reachSet[magicIndex] = reach;
        }
    }
    return kingReaches;
}

std::array<std::vector<Bitboard>, 64> computeLevelRays() {
    std::array<std::vector<Bitboard>, 64> rays;
    for (int king = A1; king <= H8; king++) {
        std::vector<Bitboard> rays2;
        for (int piece = A1; piece <= H8; piece++) {
            Bitboard ray = 0ULL;
            if (king % 8 == piece % 8) {
                for (int i = piece; i != king;  king > piece ? i += 8 : i -= 8) {
                    ray |= 1ULL << i;
                }
            } else if (king / 8 == piece / 8) {
                for (int i = piece; i != king;  king > piece ? i++ : i--) {
                    ray |= 1ULL << i;
                }
            }
            rays2.push_back(ray);
        }
        rays[king] = rays2;
    }
    return rays;
}

std::array<std::vector<Bitboard>, 64> computeDiagonalRays() {
    std::array<std::vector<Bitboard>, 64> rays;
    for (int king = A1; king <= H8; king++) {
        std::vector<Bitboard> rays2;
        for (int piece = A1; piece <= H8; piece++) {
            Bitboard ray = 0ULL;
            if (std::abs(king / 8 - piece / 8) == std::abs(king % 8 - piece % 8) && king != piece) {
                int inc = king > piece ? (king % 8 > piece % 8 ? 9 : 7) : (king % 8 > piece % 8 ? -7 : -9);
                for (int i = piece; i != king; i += inc) {
                    ray |= 1ULL << i;
                }
            }
            rays2.push_back(ray);
        }
        rays[king] = rays2;
    }
    return rays;
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
    std::array<int, 4> curr = {0, 0, 0, 0}; // CHECK might be able to get rid of this by making it pass by value instead
    generateCombination(sizes, curr, res);
    return res;
}

void generateLoneSquares(std::array<int, 4> sizes, std::vector<std::array<int, 4>>& res) {
    for (uint i = 0; i < sizes.size(); i++) {
        for (int j = 0; j <= sizes[i]; j++) {
            std::array<int, 4> combo = {0, 0, 0, 0};
            combo[i] = j;
            if (std::find(res.begin(), res.end(), combo) == res.end()) {
                res.push_back(combo);
            }
        }
    }
}

void generatePairSquares(std::array<int, 4> sizes, std::array<int, 4>& curr, std::vector<std::array<int, 4>>& res) {
    // Find limiting ray length and indices of pairs to increment.
    int smallerSize;
    int first = -1, second = -1;
    for (uint i = 0; i < sizes.size(); i++) {
        if (first == -1 && sizes[i]) {
            smallerSize = sizes[i];
            first = i;
        } else if (sizes[i]) {
            smallerSize = std::min(smallerSize, sizes[i]);
            second = i;
            break;
        }
    }

    // There are no pairs to generate which are both greater than 0.
    if (first == -1 || second == -1) return;

    // Generate the pairs
    for (int i = 0; i <= smallerSize; i++) {
        std::array<int, 4> combo = {0, 0, 0, 0};
        combo[first] = i;
        combo[second] = i;
        if (std::find(res.begin(), res.end(), combo) == res.end()) {
            res.push_back(combo);
        }
    }
}

std::vector<std::array<int, 4>> getEndBlockSquares(std::array<int, 4> sizes) {
    std::vector<std::array<int, 4>> res;
    std::array<int, 4> curr = {0, 0, 0, 0}; // CHECK might be able to get rid of this by making it pass by value instead
    generateLoneSquares(sizes, res);
    generatePairSquares({sizes[0], sizes[1], 0, 0}, curr, res);
    generatePairSquares({0, sizes[1], sizes[2], 0}, curr, res);
    generatePairSquares({0, 0, sizes[2], sizes[3]}, curr, res);
    generatePairSquares({sizes[0], 0, 0, sizes[3]}, curr, res);
    return res;
}