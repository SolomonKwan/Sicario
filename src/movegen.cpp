
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "movegen.hpp"
#include "game.hpp"
#include "utils.hpp"

MoveFamilies computeKingMoves() {
	MoveFamilies kingMoves;
	for (Square square = A1; square <= H8; square++) {
		MoveFamily& moveSet = kingMoves[square];
		std::vector<Square> destinations;
		if (rank(square) != RANK_8) destinations.push_back(square + N);
		if (rank(square) != RANK_8 && file(square) != FILE_H) destinations.push_back(square + NE);
		if (file(square) != FILE_H) destinations.push_back(square + E);
		if (rank(square) != RANK_1 && file(square) != FILE_H) destinations.push_back(square + SE);
		if (rank(square) != RANK_1) destinations.push_back(square + S);
		if (rank(square) != RANK_1 && file(square) != FILE_A) destinations.push_back(square + SW);
		if (file(square) != FILE_A) destinations.push_back(square + W);
		if (rank(square) != RANK_8 && file(square) != FILE_A) destinations.push_back(square + NW);

		uint16_t maxOccupancy = (uint16_t)pow(2, destinations.size());
		moveSet.resize(maxOccupancy);

		// Build occupancy bitboard
		for (uint16_t j = 0; j < maxOccupancy; j++) {
			MoveVector moves;

			uint64_t occ = ZERO_BB;
			int shift = 0;
			for (Square dest : destinations) {
				if ((j >> shift) & 1UL) {
					moves.push_back(square | (dest << DEST_SHIFT));
				}
				occ |= ((j >> shift) & 1UL) << dest;
				shift++;
			}

			uint16_t magicIndex = getKingMovesIndex(occ, square);
			moveSet[magicIndex] = moves;
		}
	}
	return kingMoves;
}

MoveFamilies computeKnightMoves() {
	MoveFamilies knightMoves;
	for (Square square = A1; square <= H8; square++) {
		MoveFamily& moveSet = knightMoves[square];
		std::vector<Square> destinations;
		if (rank(square) < RANK_7 && file(square) < FILE_H) destinations.push_back(square + NNE);
		if (rank(square) < RANK_8 && file(square) < FILE_G) destinations.push_back(square + ENE);
		if (rank(square) > RANK_1 && file(square) < FILE_G) destinations.push_back(square + ESE);
		if (rank(square) > RANK_2 && file(square) < FILE_H) destinations.push_back(square + SES);
		if (rank(square) > RANK_2 && file(square) > FILE_A) destinations.push_back(square + SWS);
		if (rank(square) > RANK_1 && file(square) > FILE_B) destinations.push_back(square + WSW);
		if (rank(square) < RANK_8 && file(square) > FILE_B) destinations.push_back(square + WNW);
		if (rank(square) < RANK_7 && file(square) > FILE_A) destinations.push_back(square + NWN);

		uint16_t maxOccupancy = (uint16_t)pow(2, destinations.size());
		moveSet.resize(maxOccupancy);

		// Build occupancy bitboard
		for (uint16_t j = 0; j < maxOccupancy; j++) {
			MoveVector moves;

			uint64_t occ = ZERO_BB;
			int shift = 0;
			for (Square dest : destinations) {
				if ((j >> shift) & 1UL) {
					moves.push_back(square | (dest << DEST_SHIFT));
				}
				occ |= ((j >> shift) & 1UL) << dest;
				shift++;
			}

			uint16_t magicIndex = getKnightMovesIndex(occ, square);
			moveSet[magicIndex] = moves;
		}
	}
	return knightMoves;
}

std::array<MoveFamilies, PLAYER_COUNT> computePawnMoves() {
	std::array<MoveFamilies, PLAYER_COUNT> pawnMoves = {
		computePawnMovesBySide(BLACK),
		computePawnMovesBySide(WHITE)
	};
	return pawnMoves;
}

MoveFamilies computePawnMovesBySide(const Player player) {
	MoveFamilies pawnMoves;
	for (Square square = A2; square <= H7; square++) {
		MoveFamily& moveSet = pawnMoves[square];
		std::vector<Square> destinations;
		if (player == BLACK) {
			destinations.push_back(square + S);
			if (rank(square) == RANK_7) destinations.push_back(square + SS);
			if (file(square) != FILE_A) destinations.push_back(square + SW);
			if (file(square) != FILE_H) destinations.push_back(square + SE);
		} else {
			destinations.push_back(square + N);
			if (rank(square) == RANK_2) destinations.push_back(square + NN);
			if (file(square) != FILE_A) destinations.push_back(square + NW);
			if (file(square) != FILE_H) destinations.push_back(square + NE);
		}

		int totalSize = destinations.size();
		uint16_t maxOccupancy = (uint16_t)pow(2, totalSize);
		moveSet.resize(maxOccupancy);

		for (uint16_t j = 0; j < maxOccupancy; j++) {
			MoveVector moves;

			uint64_t occ = ZERO_BB;
			int shift = 0;
			for (Square dest : destinations) {
				if ((j >> shift) & 1UL) {
					if (rank(dest) == RANK_1 || rank(dest) == RANK_8) {
						moves.push_back(square | (dest << DEST_SHIFT) | PROMOTION | pQUEEN);
						moves.push_back(square | (dest << DEST_SHIFT) | PROMOTION | pROOK);
						moves.push_back(square | (dest << DEST_SHIFT) | PROMOTION | pBISHOP);
						moves.push_back(square | (dest << DEST_SHIFT) | PROMOTION | pKNIGHT);
					} else {
						moves.push_back(square | (dest << DEST_SHIFT));
					}
				}
				occ |= ((j >> shift) & 1UL) << dest;
				shift++;
			}

			uint16_t magicIndex = getPawnMovesIndex(occ, square, player);
			moveSet[magicIndex] = moves;
		}
	}
	return pawnMoves;
}

MoveFamilies computeRookMoves() {
	MoveFamilies rookMoves;
	for (Square square = A1; square <= H8; square++) {
		int northSize = std::max(static_cast<int>(RANK_8 - rank(square)), 0);
		int southSize = std::max(static_cast<int>(rank(square)), 0);
		int eastSize = std::max(static_cast<int>(RANK_8 - file(square)), 0);
		int westSize = std::max(static_cast<int>(file(square)), 0);
		MoveFamily& movesSet = rookMoves[square];
		movesSet.resize((int)std::pow(2, SQUARE_COUNT - Shifts::Moves::ROOK[square]));

		for (std::array<int, 4> selection : getEndCombinations({northSize, eastSize, southSize, westSize})) {
			uint64_t reach = ZERO_BB;
			MoveVector moves;

			for (int i = 0; i < selection[0]; i++) {
				reach |= ONE_BB << (square + N * (i + 1));
				moves.push_back(square | ((square + N * (i + 1)) << DEST_SHIFT));
			}

			for (int i = 0; i < selection[1]; i++) {
				reach |= ONE_BB << (square + E * (i + 1));
				moves.push_back(square | ((square + E * (i + 1)) << DEST_SHIFT));
			}

			for (int i = 0; i < selection[2]; i++) {
				reach |= ONE_BB << (square + S * (i + 1));
				moves.push_back(square | ((square + S * (i + 1)) << DEST_SHIFT));
			}

			for (int i = 0; i < selection[3]; i++) {
				reach |= ONE_BB << (square + W * (i + 1));
				moves.push_back(square | ((square + W * (i + 1)) << DEST_SHIFT));
			}

			int magicIndex = getRookMovesIndex(reach, square);
			movesSet[magicIndex] = moves;
		}
	}
	return rookMoves;
}

MoveFamilies computeBishopMoves() {
	MoveFamilies bishopMoves;
	for (Square square = A1; square <= H8; square++) {
		int northEastSize = std::max(std::min(RANK_8 - rank(square), FILE_H - file(square)), 0);
		int southEastSize = std::max(std::min(static_cast<int>(rank(square)), FILE_H - file(square)), 0);
		int southWestSize = std::max(std::min(static_cast<int>(rank(square)), static_cast<int>(file(square))), 0);
		int northWestSize = std::max(std::min(RANK_8 - static_cast<int>(rank(square)), static_cast<int>(file(square))), 0);
		MoveFamily& movesSet = bishopMoves[square];
		movesSet.resize((int)std::pow(2, SQUARE_COUNT - Shifts::Moves::BISHOP[square]));

		for (std::array<int, 4> selection : getEndCombinations({northEastSize, southEastSize, southWestSize,
				northWestSize})) {
			uint64_t reach = ZERO_BB;
			MoveVector moves;

			for (int i = 0; i < selection[0]; i++) {
				reach |= ONE_BB << (square + NE * (i + 1));
				moves.push_back(square | ((square + NE * (i + 1)) << DEST_SHIFT));
			}

			for (int i = 0; i < selection[1]; i++) {
				reach |= ONE_BB << (square + SE * (i + 1));
				moves.push_back(square | ((square + SE * (i + 1)) << DEST_SHIFT));
			}

			for (int i = 0; i < selection[2]; i++) {
				reach |= ONE_BB << (square + SW * (i + 1));
				moves.push_back(square | ((square + SW * (i + 1)) << DEST_SHIFT));
			}

			for (int i = 0; i < selection[3]; i++) {
				reach |= ONE_BB << (square + NW * (i + 1));
				moves.push_back(square | ((square + NW * (i + 1)) << DEST_SHIFT));
			}

			int magicIndex = getBishopMovesIndex(reach, square);
			movesSet[magicIndex] = moves;
		}
	}
	return bishopMoves;
}

std::array<MoveVector, CASTLING_OPTIONS> computeCastlingMoves() {
	std::array<MoveVector, CASTLING_OPTIONS> moves;
	moves[WKSC] = {E1 | G1 << DEST_SHIFT | CASTLING};
	moves[WQSC] = {E1 | C1 << DEST_SHIFT | CASTLING};
	moves[BKSC] = {E8 | G8 << DEST_SHIFT | CASTLING};
	moves[BQSC] = {E8 | C8 << DEST_SHIFT | CASTLING};
	return moves;
}

std::array<std::vector<MoveFamily>, PLAYER_COUNT> computeEnPassantMoves() {
	std::array<std::vector<MoveFamily>, PLAYER_COUNT> moves;
	std::vector<MoveFamily>& blackMove = moves[BLACK];
	std::vector<MoveFamily>& whiteMove = moves[WHITE];
	blackMove = {
		{{B4 | A3 << DEST_SHIFT | EN_PASSANT}, {B4 | A3 << DEST_SHIFT | EN_PASSANT}},
		{{A4 | B3 << DEST_SHIFT | EN_PASSANT}, {C4 | B3 << DEST_SHIFT | EN_PASSANT}},
		{{B4 | C3 << DEST_SHIFT | EN_PASSANT}, {D4 | C3 << DEST_SHIFT | EN_PASSANT}},
		{{C4 | D3 << DEST_SHIFT | EN_PASSANT}, {E4 | D3 << DEST_SHIFT | EN_PASSANT}},
		{{D4 | E3 << DEST_SHIFT | EN_PASSANT}, {F4 | E3 << DEST_SHIFT | EN_PASSANT}},
		{{E4 | F3 << DEST_SHIFT | EN_PASSANT}, {G4 | F3 << DEST_SHIFT | EN_PASSANT}},
		{{F4 | G3 << DEST_SHIFT | EN_PASSANT}, {H4 | G3 << DEST_SHIFT | EN_PASSANT}},
		{{G4 | H3 << DEST_SHIFT | EN_PASSANT}, {G4 | H3 << DEST_SHIFT | EN_PASSANT}}
	};
	whiteMove = {
		{{B5 | A6 << DEST_SHIFT | EN_PASSANT}, {B5 | A6 << DEST_SHIFT | EN_PASSANT}},
		{{A5 | B6 << DEST_SHIFT | EN_PASSANT}, {C5 | B6 << DEST_SHIFT | EN_PASSANT}},
		{{B5 | C6 << DEST_SHIFT | EN_PASSANT}, {D5 | C6 << DEST_SHIFT | EN_PASSANT}},
		{{C5 | D6 << DEST_SHIFT | EN_PASSANT}, {E5 | D6 << DEST_SHIFT | EN_PASSANT}},
		{{D5 | E6 << DEST_SHIFT | EN_PASSANT}, {F5 | E6 << DEST_SHIFT | EN_PASSANT}},
		{{E5 | F6 << DEST_SHIFT | EN_PASSANT}, {G5 | F6 << DEST_SHIFT | EN_PASSANT}},
		{{F5 | G6 << DEST_SHIFT | EN_PASSANT}, {H5 | G6 << DEST_SHIFT | EN_PASSANT}},
		{{G5 | H6 << DEST_SHIFT | EN_PASSANT}, {G5 | H6 << DEST_SHIFT | EN_PASSANT}}
	};
	return moves;
}

MoveFamilies computeRookBlockMoves() {
	MoveFamilies rookBlockMoves;
	for (Square square = A1; square <= H8; square++) {
		int northSize = std::max(RANK_8 - rank(square), 0);
		int southSize = std::max(static_cast<int>(rank(square)), 0);
		int eastSize = std::max(FILE_H - file(square), 0);
		int westSize = std::max(static_cast<int>(file(square)), 0);
		MoveFamily& movesSet = rookBlockMoves[square];
		movesSet.resize((int)std::pow(2, SQUARE_COUNT - Shifts::Block::ROOK[square]));

		for (std::array<int, 4> selection : getEndBlockSquares({northSize, eastSize, southSize, westSize})) {
			MoveVector moves;
			uint64_t occ = ZERO_BB;
			if (selection[0]) {
				moves.push_back(square | (square + (N * selection[0])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (N * selection[0]));
			}

			if (selection[1]) {
				moves.push_back(square | (square + (E * selection[1])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (E * selection[1]));
			}

			if (selection[2]) {
				moves.push_back(square | (square + (S * selection[2])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (S * selection[2]));
			}

			if (selection[3]) {
				moves.push_back(square | (square + (W * selection[3])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (W * selection[3]));
			}

			movesSet[getRookBlockIndex(occ, square)] = moves;
		}
	}
	return rookBlockMoves;
}

MoveFamilies computeBishopBlockMoves() {
	MoveFamilies bishopBlockMoves;
	for (Square square = A1; square <= H8; square++) {
		int northEastSize = std::max(std::min(RANK_8 - rank(square), RANK_8 - file(square)), 0);
		int southEastSize = std::max(std::min(static_cast<int>(rank(square)), FILE_H - file(square)), 0);
		int southWestSize = std::max(std::min(static_cast<int>(rank(square)), static_cast<int>(file(square))), 0);
		int northWestSize = std::max(std::min(RANK_8 - rank(square), static_cast<int>(file(square))), 0);
		MoveFamily& movesSet = bishopBlockMoves[square];
		movesSet.resize((int)std::pow(2, SQUARE_COUNT - Shifts::Block::BISHOP[square]));

		for (std::array<int, 4> selection : getEndBlockSquares({northEastSize, southEastSize, southWestSize,
				northWestSize})) {
			MoveVector moves;
			uint64_t occ = ZERO_BB;
			if (selection[0]) {
				moves.push_back(square | (square + (NE * selection[0])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (NE * selection[0]));
			}

			if (selection[1]) {
				moves.push_back(square | (square + (SE * selection[1])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (SE * selection[1]));
			}

			if (selection[2]) {
				moves.push_back(square | (square + (SW * selection[2])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (SW * selection[2]));
			}

			if (selection[3]) {
				moves.push_back(square | (square + (NW * selection[3])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (NW * selection[3]));
			}

			movesSet[getBishopBlockIndex(occ, square)] = moves;
		}
	}
	return bishopBlockMoves;
}

IndicesFamily computeRookReachIndices() {
	IndicesFamily rookIndices;
	for (Square square = A1; square <= H8; square++) {
		std::vector<uint>& indices = rookIndices[square];
		int northSize = std::max(6 - rank(square), 0);
		int southSize = std::max(rank(square) - 1, 0);
		int eastSize = std::max(6 - file(square), 0);
		int westSize = std::max(file(square) - 1, 0);
		int totalSize = northSize + southSize + eastSize + westSize;
		indices.resize(pow(2, totalSize));

		uint16_t maxOccupancy = pow(2, totalSize);
		for (uint16_t j = 0; j < maxOccupancy; j++) {
			int northBlock = 0, eastBlock = 0, southBlock = 0, westBlock = 0;
			uint64_t occ = ZERO_BB;
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

			uint16_t magicIndex = getRookReachIndex(occ, square);
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

IndicesFamily computeBishopReachIndices() {
	IndicesFamily bishopIndices;
	for (Square square = A1; square <= H8; square++) {
		std::vector<uint>& indices = bishopIndices[square];
		int northeastSize = std::max(std::min(6 - rank(square), 6 - file(square)), 0);
		int southeastSize = std::max(std::min(rank(square) - 1, 6 - file(square)), 0);
		int southwestSize = std::max(std::min(rank(square) - 1, file(square) - 1), 0);
		int northwestSize = std::max(std::min(6 - rank(square), file(square) - 1), 0);
		int totalSize = northeastSize + southeastSize + southwestSize + northwestSize;
		indices.resize(pow(2, totalSize));

		uint16_t maxOccupancy = pow(2, totalSize);
		for (uint16_t j = 0; j < maxOccupancy; j++) {
			int northeastBlock = 0, southeastBlock = 0, southwestBlock = 0, northwestBlock = 0;
			uint64_t occ = ZERO_BB;
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

			uint16_t magicIndex = getBishopReachIndex(occ, square);
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

BitboardFamily computeRookReaches() {
	const IndicesFamily ROOK = computeRookReachIndices();
	BitboardFamily reaches;
	for (int square = A1; square <= H8; square++) {
		BitboardVector& reachSet = reaches[square];
		int northSize = std::max(7 - (square / 8), 0);
		int southSize = std::max(square / 8, 0);
		int eastSize = std::max(7 - (square % 8), 0);
		int westSize = std::max(square % 8, 0);
		reachSet.resize(*std::max_element(ROOK[square].begin(), ROOK[square].end()) + 1);
		std::vector<std::array<int, 4>> combos = getEndCombinations({northSize, eastSize, southSize, westSize});

		for (std::array<int, 4> combo : combos) {
			Bitboard occ = ZERO_BB;
			if (combo[0]) occ |= (ONE_BB << (square + N * combo[0]));
			if (combo[1]) occ |= (ONE_BB << (square + E * combo[1]));
			if (combo[2]) occ |= (ONE_BB << (square + S * combo[2]));
			if (combo[3]) occ |= (ONE_BB << (square + W * combo[3]));

			Bitboard reach = ZERO_BB;
			for (int i = 1; combo[0] > 0; combo[0]--, i++) {
				reach |= (ONE_BB << (square + i * N));
			}

			for (int i = 1; combo[1] > 0; combo[1]--, i++) {
				reach |= (ONE_BB << (square + i * E));
			}

			for (int i = 1; combo[2] > 0; combo[2]--, i++) {
				reach |= (ONE_BB << (square + i * S));
			}

			for (int i = 1; combo[3] > 0; combo[3]--, i++) {
				reach |= (ONE_BB << (square + i * W));
			}

			reachSet[ROOK[square][getRookReachIndex(occ & Masks::ROOK[square], (Square) square)]] = reach;
		}
	}
	return reaches;
}

BitboardFamily computeBishopReaches() {
	const IndicesFamily BISHOP = computeBishopReachIndices();
	BitboardFamily reaches;
	for (int square = A1; square <= H8; square++) {
		BitboardVector& reachSet = reaches[square];
		int northeastSize = std::max(std::min(7 - square / 8, 7 - square % 8), 0);
		int southeastSize = std::max(std::min(square / 8, 7 - square % 8), 0);
		int southwestSize = std::max(std::min(square / 8, square % 8), 0);
		int northwestSize = std::max(std::min(7 - square / 8, square % 8), 0);
		reachSet.resize(*std::max_element(BISHOP[square].begin(), BISHOP[square].end()) + 1);
		std::vector<std::array<int, 4>> combos = getEndCombinations({northeastSize, southeastSize, southwestSize,
				northwestSize});

		for (std::array<int, 4> combo : combos) {
			Bitboard occ = ZERO_BB;
			if (combo[0]) occ |= (ONE_BB << (square + NE * combo[0]));
			if (combo[1]) occ |= (ONE_BB << (square + SE * combo[1]));
			if (combo[2]) occ |= (ONE_BB << (square + SW * combo[2]));
			if (combo[3]) occ |= (ONE_BB << (square + NW * combo[3]));

			Bitboard reach = ZERO_BB;
			for (int i = 1; combo[0] > 0; combo[0]--, i++) {
				reach |= (ONE_BB << (square + i * NE));
			}

			for (int i = 1; combo[1] > 0; combo[1]--, i++) {
				reach |= (ONE_BB << (square + i * SE));
			}

			for (int i = 1; combo[2] > 0; combo[2]--, i++) {
				reach |= (ONE_BB << (square + i * SW));
			}

			for (int i = 1; combo[3] > 0; combo[3]--, i++) {
				reach |= (ONE_BB << (square + i * NW));
			}

			reachSet[BISHOP[square][getBishopReachIndex(occ & Masks::BISHOP[square], (Square) square)]] = reach;
		}
	}
	return reaches;
}

std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> computeKingReachSquares() {
	std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> kingReaches;
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

			uint64_t occ = ZERO_BB;
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

BitboardFamily computeLevelRays() {
	BitboardFamily rays;
	for (int king = A1; king <= H8; king++) {
		BitboardVector rays2;
		for (int piece = A1; piece <= H8; piece++) {
			Bitboard ray = ZERO_BB;
			if (king % 8 == piece % 8) {
				for (int i = piece; i != king;  king > piece ? i += 8 : i -= 8) {
					ray |= ONE_BB << i;
				}
			} else if (king / 8 == piece / 8) {
				for (int i = piece; i != king;  king > piece ? i++ : i--) {
					ray |= ONE_BB << i;
				}
			}
			rays2.push_back(ray);
		}
		rays[king] = rays2;
	}
	return rays;
}

BitboardFamily computeDiagonalRays() {
	BitboardFamily rays;
	for (int king = A1; king <= H8; king++) {
		BitboardVector rays2;
		for (int piece = A1; piece <= H8; piece++) {
			Bitboard ray = ZERO_BB;
			if (std::abs(king / 8 - piece / 8) == std::abs(king % 8 - piece % 8) && king != piece) {
				int inc = king > piece ? (king % 8 > piece % 8 ? 9 : 7) : (king % 8 > piece % 8 ? -7 : -9);
				for (int i = piece; i != king; i += inc) {
					ray |= ONE_BB << i;
				}
			}
			rays2.push_back(ray);
		}
		rays[king] = rays2;
	}
	return rays;
}

int getIndex(const std::vector<int> values, const std::vector<int> ranges) {
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

int pairingFunction(const int n, const int m, const int N, const int M) {
	if (N == M) {
		return n + m * N;
	} else if (N < M) {
		return m + n * M;
	} else {
		return n + m * N;
	}
}

void generateCombination(std::array<int, 4> sizes, std::array<int, 4> curr, std::vector<std::array<int, 4>>& res) {
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
	generateCombination(sizes, {0, 0, 0, 0}, res);
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

void generatePairSquares(std::array<int, 4> sizes, std::array<int, 4> curr, std::vector<std::array<int, 4>>& res) {
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
	generateLoneSquares(sizes, res);
	generatePairSquares({sizes[0], sizes[1], 0, 0}, {0, 0, 0, 0}, res);
	generatePairSquares({0, sizes[1], sizes[2], 0}, {0, 0, 0, 0}, res);
	generatePairSquares({0, 0, sizes[2], sizes[3]}, {0, 0, 0, 0}, res);
	generatePairSquares({sizes[0], 0, 0, sizes[3]}, {0, 0, 0, 0}, res);
	return res;
}