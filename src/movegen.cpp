
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <numeric>

#include "movegen.hpp"
#include "game.hpp"
#include "utils.hpp"

typedef std::array<int, 4> ArrayFour;

struct RaysPairs {
	ArrayFour sizes;
	ArrayFour directions;
};

typedef void (*GetSquares)(const Square, std::vector<Square>&);
typedef uint (*GetMovesIndex)(const Bitboard reach, const Square square);
typedef RaysPairs (*GetRayPairs)(const Square square);
typedef IndicesFamily (*ReachIndices)();

/**
 * @brief Populate the destinations vector with the squares that a king can reach from the specified position.
 *
 * @param square The square that the king is on.
 * @param destinations The destinations vector to populate.
 */
void getKingSquares(const Square square, std::vector<Square>& destinations) {
	if (rank(square) != RANK_8) destinations.push_back(square + N);
	if (file(square) != FILE_H) destinations.push_back(square + E);
	if (rank(square) != RANK_1) destinations.push_back(square + S);
	if (file(square) != FILE_A) destinations.push_back(square + W);
	if (rank(square) != RANK_8 && file(square) != FILE_H) destinations.push_back(square + NE);
	if (rank(square) != RANK_1 && file(square) != FILE_H) destinations.push_back(square + SE);
	if (rank(square) != RANK_1 && file(square) != FILE_A) destinations.push_back(square + SW);
	if (rank(square) != RANK_8 && file(square) != FILE_A) destinations.push_back(square + NW);
}

/**
 * @brief Populate the destinations vector with the squares that a knight can reach from the specified position.
 *
 * @param square The square that the knight is on.
 * @param destinations The destinations vector to populate.
 */
void getKnightSquares(const Square square, std::vector<Square>& destinations) {
	if (rank(square) < RANK_7 && file(square) < FILE_H) destinations.push_back(square + NNE);
	if (rank(square) < RANK_8 && file(square) < FILE_G) destinations.push_back(square + ENE);
	if (rank(square) > RANK_1 && file(square) < FILE_G) destinations.push_back(square + ESE);
	if (rank(square) > RANK_2 && file(square) < FILE_H) destinations.push_back(square + SES);
	if (rank(square) > RANK_2 && file(square) > FILE_A) destinations.push_back(square + SWS);
	if (rank(square) > RANK_1 && file(square) > FILE_B) destinations.push_back(square + WSW);
	if (rank(square) < RANK_8 && file(square) > FILE_B) destinations.push_back(square + WNW);
	if (rank(square) < RANK_7 && file(square) > FILE_A) destinations.push_back(square + NWN);
}

/**
 * @brief Populate the destinations vector with the squares that a pawn can reach form the speciifed position.
 *
 * @param square The square that the pawn is on.
 * @param player The player side.
 * @param destinations The destinations vector to populate.
 */
void getPawnSquares(const Square square, const Player player, std::vector<Square>& destinations) {
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
}

/**
 * @brief Populate the move vector with the moves of a pawn from the start to dest squares.
 *
 * @param start The starting square of a pawn.
 * @param dest The end quare of the a pawn.
 * @param moves The moves vector to populate.
 */
void pawnMoveGenerator(const Square start, const Square end, MoveVector& moves) {
	if (rank(end) == RANK_1 || rank(end) == RANK_8) {
		moves.push_back(start | (end << DEST_SHIFT) | PROMOTION | pQUEEN);
		moves.push_back(start | (end << DEST_SHIFT) | PROMOTION | pROOK);
		moves.push_back(start | (end << DEST_SHIFT) | PROMOTION | pBISHOP);
		moves.push_back(start | (end << DEST_SHIFT) | PROMOTION | pKNIGHT);
	} else {
		moves.push_back(start | (end << DEST_SHIFT));
	}
}

/**
 * @brief Get the size of the north, east, south and west rays of a rook on a given square.
 *
 * @param square The square that the rook is on.
 * @return A RaysPairs struct containg the sizes and directions of each ray.
 */
RaysPairs getRookRayPairs(const Square square) {
	int northSize = RANK_8 - rank(square);
	int eastSize  = FILE_H - file(square);
	int southSize = rank(square);
	int westSize  = file(square);
	return {
		{northSize, eastSize, southSize, westSize},
		{N, E, S, W}
	};
}

/**
 * @brief Get the size of the northeast, southeast, southwest and northwest rays of a bishop on a given square.
 *
 * @param square The square that the bishop is on.
 * @return A RaysPairs struct containing the sizes and directions of each ray.
 */
RaysPairs getBishopRayPairs(const Square square) {
	int northEastSize = std::min(RANK_8 - rank(square), FILE_H - file(square));
	int southEastSize = std::min(static_cast<int>(rank(square)), FILE_H - file(square));
	int southWestSize = std::min(static_cast<int>(rank(square)), static_cast<int>(file(square)));
	int northWestSize = std::min(RANK_8 - rank(square), static_cast<int>(file(square)));
	return {
		{northEastSize, southEastSize, southWestSize, northWestSize},
		{NE, SE, SW, NW}
	};
}

/**
 * @brief Get the size of the north, east, south and west index rays of a rook on a given square.
 *
 * @param square The square that the rook is on.
 * @return A RaysPairs struct containing the sizes and directions of each ray.
 */
RaysPairs rookReachIndexRaySizes(const Square square) {
	int northSize = std::max(RANK_7 - rank(square), 0);
	int eastSize  = std::max(FILE_G - file(square), 0);
	int southSize = std::max(rank(square) - RANK_2, 0);
	int westSize  = std::max(file(square) - FILE_B, 0);
	return {
		{northSize, eastSize, southSize, westSize},
		{N, E, S, W}
	};
}

/**
 * @brief Get the size of the northeast, southeast, southwest and northwest rays of a bishop on a given square.
 *
 * @param square The square that the bishop is on.
 * @return A RaysPairs struct containing the sizes and directions of each ray.
 */
RaysPairs bishopReachIndexRaySizes(const Square square) {
	int northeastSize = std::max(std::min(RANK_7 - rank(square), FILE_G - file(square)), 0);
	int southeastSize = std::max(std::min(rank(square) - RANK_2, FILE_G - file(square)), 0);
	int southwestSize = std::max(std::min(rank(square) - RANK_2, file(square) - FILE_B), 0);
	int northwestSize = std::max(std::min(RANK_7 - rank(square), file(square) - FILE_B), 0);
	return {
		{northeastSize, southeastSize, southwestSize, northwestSize},
		{NE, SE, SW, NW}
	};
}

/**
 * @brief Get the size of the north, east, south and west reach rays of a rook on a given square.
 *
 * @param square The square that the rook is on.
 * @return A RaysPairs struct containing the sizes and directions of each ray.
 */
RaysPairs rookReachRaySizes(const Square square) {
	int northSize = RANK_8 - (square / RANK_COUNT);
	int eastSize  = FILE_H - (square % FILE_COUNT);
	int southSize = square / RANK_COUNT;
	int westSize  = square % FILE_COUNT;
	return {
		{northSize, eastSize, southSize, westSize},
		{N, E, S, W}
	};
}

/**
 * @brief Get the size of the northeast, southeast, southwest and northwest reach rays of a bishop on a given square.
 *
 * @param square The square that the bishop is on.
 * @return A RaysPairs struct containing the sizes and directions of each ray.
 */
RaysPairs bishopReachRaySizes(const Square square) {
	int northeastSize = std::min(RANK_8 - square / RANK_COUNT, FILE_H - square % FILE_COUNT);
	int southeastSize = std::min(square / RANK_COUNT, FILE_H - square % FILE_COUNT);
	int southwestSize = std::min(square / RANK_COUNT, square % FILE_COUNT);
	int northwestSize = std::min(RANK_8 - square / RANK_COUNT, square % FILE_COUNT);
	return {
		{northeastSize, southeastSize, southwestSize, northwestSize},
		{NE, SE, SW, NW}
	};
}

/**
 * @brief Compute and return the moves for static/non-ranged pieces.
 *
 * @param getSquares Function pointer to the neighbouring square generator.
 * @param getMovesIndex Function pointer to the moves index getter.
 * @return MoveFamilies of the piece moves on all squares and occupancies.
 */
MoveFamilies computeStaticMoves(GetSquares getSquares, GetMovesIndex getMovesIndex, bool isPawn, Player player) {
	MoveFamilies moves;
	for (Square square = A1; square <= H8; square++) {
		MoveFamily& moveSet = moves[square];

		// Get the destination squares of a piece on the square.
		std::vector<Square> destinations;
		if (isPawn == false)
			getSquares(square, destinations);
		else
			getPawnSquares(square, player, destinations);

		uint16_t maxOccupancy = static_cast<uint16_t>(pow(2, destinations.size()));
		moveSet.resize(maxOccupancy);

		// Iterate every possible occupancy.
		for (uint16_t num = 0; num < maxOccupancy; num++) {
			MoveVector moves;
			uint64_t occ = ZERO_BB;
			int shift = 0;

			// Build occupancy bitboard.
			for (Square dest : destinations) {
				if (isSet<uint16_t>(num, shift) && isPawn == false)
					moves.push_back(square | (dest << DEST_SHIFT));
				else if (isSet<uint16_t>(num, shift) && isPawn == true)
					pawnMoveGenerator(square, dest, moves);
				occ |= (extractBit<uint16_t>(num, shift) << dest);
				shift++;
			}

			if (isPawn == false)
				moveSet[getMovesIndex(occ, square)] = moves;
			else
				moveSet[getPawnMovesIndex(occ, square, player)] = moves;
		}
	}
	return moves;
}

/**
 * @brief Compute and return the moves for ranged pieces.
 *
 * @param getRayPairs Function pointer to the appropriate RayPair getter function.
 * @param getMovesIndex Function pointer to the moves index getter.
 * @param shift The corresponding shift array.
 * @return MoveFamilies of the ranged  moves on all squares and occupancies.
 */
MoveFamilies computeRangedMoves(GetRayPairs getRayPairs, GetMovesIndex getMovesIndex, const int shift[SQUARE_COUNT]) {
	MoveFamilies moves;
	for (Square square = A1; square <= H8; square++) {
		MoveFamily& movesSet = moves[square];
		movesSet.resize(std::pow(2, SQUARE_COUNT - shift[square]));
		RaysPairs pair = getRayPairs(square);

		// Iterate all end blocking combinations.
		for (ArrayFour selection : getEndCombinations(pair.sizes)) {
			uint64_t reach = ZERO_BB;
			MoveVector moves;

			// Iterate each ray direction.
			for (int rayIndex = 0; rayIndex < 4; rayIndex++) {

				// Build each ray and corresponding moves.
				for (int iter = 1; iter <= selection[rayIndex]; iter++) {
					reach |= ONE_BB << (square + pair.directions[rayIndex] * iter);
					moves.push_back(square | ((square + pair.directions[rayIndex] * iter) << DEST_SHIFT));
				}
			}

			int magicIndex = getMovesIndex(reach, square);
			movesSet[magicIndex] = moves;
		}
	}
	return moves;
}

MoveFamilies computeKingMoves() {
	return computeStaticMoves(&getKingSquares, &getKingMovesIndex, false, WHITE);
}

MoveFamilies computeRookMoves() {
	return computeRangedMoves(getRookRayPairs, getRookMovesIndex, Shifts::Moves::ROOK);
}

MoveFamilies computeBishopMoves() {
	return computeRangedMoves(getBishopRayPairs, getBishopMovesIndex, Shifts::Moves::BISHOP);
}

MoveFamilies computeKnightMoves() {
	return computeStaticMoves(&getKnightSquares, &getKnightMovesIndex, false, WHITE);
}

std::array<MoveFamilies, PLAYER_COUNT> computePawnMoves() {
	std::array<MoveFamilies, PLAYER_COUNT> pawnMoves = {
		computePawnMovesBySide(BLACK),
		computePawnMovesBySide(WHITE)
	};
	return pawnMoves;
}

MoveFamilies computePawnMovesBySide(const Player player) {
	return computeStaticMoves(nullptr, nullptr, true, player);
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

MoveFamilies computeBlockMoves(GetRayPairs getRayPairs, GetMovesIndex getMovesIndex, const int shift[SQUARE_COUNT]) {
	MoveFamilies moves;
	for (Square square = A1; square <= H8; square++) {
		MoveFamily& movesSet = moves[square];
		movesSet.resize((int)std::pow(2, SQUARE_COUNT - shift[square]));
		RaysPairs pair = getRayPairs(square);

		// Iterate blocking combinations
		for (ArrayFour selection : getEndBlockSquares(pair.sizes)) {
			MoveVector moves;
			uint64_t occ = ZERO_BB;

			// Iterate each ray
			for (int index = 0; index < 4; index++) {
				if (selection[index] == 0) continue;

				// Build move array and occupancy bitboard.
				moves.push_back(square | (square + (pair.directions[index] * selection[index])) << DEST_SHIFT);
				occ |= ONE_BB << (square + (pair.directions[index] * selection[index]));
			}

			movesSet[getMovesIndex(occ, square)] = moves;
		}
	}
	return moves;
}

MoveFamilies computeRookBlockMoves() {
	return computeBlockMoves(getRookRayPairs, getRookBlockIndex, Shifts::Block::ROOK);
}

MoveFamilies computeBishopBlockMoves() {
	return computeBlockMoves(getBishopRayPairs, getBishopBlockIndex, Shifts::Block::BISHOP);
}

IndicesFamily computeReachIndices(GetRayPairs getRayPairs, GetMovesIndex getMovesIndex) {
	IndicesFamily pieceIndices;
	for (Square square = A1; square <= H8; square++) {
		RaysPairs pairs = getRayPairs(square);
		int totalSize = std::accumulate(pairs.sizes.begin(), pairs.sizes.end(), 0);
		std::vector<uint>& indices = pieceIndices[square];
		indices.resize(pow(2, totalSize));
		uint16_t maxOccupancy = pow(2, totalSize);

		// Iterate all occupancies
		for (uint16_t num = 0; num < maxOccupancy; num++) {
			uint64_t occ = ZERO_BB;
			int shift = 0;
			ArrayFour blockers = {0};

			// Iterate each ray
			for (int rayIndex = 0; rayIndex < 4; rayIndex++) {

				// Build occupancy bitboard.
				for (int iter = 1; iter <= pairs.sizes[rayIndex]; iter++, shift++) {
					occ |= extractBit<uint16_t>(num, shift) << (square + pairs.directions[rayIndex] * iter);
					if (!blockers[rayIndex] && isSet<uint16_t>(num, shift))
						blockers[rayIndex] = iter;
				}
			}

			uint16_t magicIndex = getMovesIndex(occ, square);
			int mappedIndex = getIndex(
				{blockers[0], blockers[1], blockers[2], blockers[3]},
				{
					pairs.sizes[0] ? pairs.sizes[0] + 1 : 0,
					pairs.sizes[1] ? pairs.sizes[1] + 1 : 0,
					pairs.sizes[2] ? pairs.sizes[2] + 1 : 0,
					pairs.sizes[3] ? pairs.sizes[3] + 1 : 0
				}
			);
			indices[magicIndex] = mappedIndex;
		}
	}
	return pieceIndices;
}

IndicesFamily computeRookReachIndices() {
	return computeReachIndices(rookReachIndexRaySizes, getRookReachIndex);
}

IndicesFamily computeBishopReachIndices() {
	return computeReachIndices(bishopReachIndexRaySizes, getBishopReachIndex);
}

BitboardFamily computeReaches(ReachIndices getReachIndices, GetRayPairs getRayPairs, GetMovesIndex getReachIndex,
		const Bitboard mask[SQUARE_COUNT]) {
	const IndicesFamily PIECE = getReachIndices();
	BitboardFamily reaches;
	for (Square square = A1; square <= H8; square++) {
		BitboardVector& reachSet = reaches[square];
		reachSet.resize(*std::max_element(PIECE[square].begin(), PIECE[square].end()) + 1);
		RaysPairs pairs = getRayPairs(square);
		std::vector<ArrayFour> combos = getEndCombinations(pairs.sizes);

		// Iterate all end blocking combinations.
		for (ArrayFour combo : combos) {
			Bitboard occ = ZERO_BB;
			Bitboard reach = ZERO_BB;

			// Iterate each ray
			for (int index = 0; index < 4; index++) {
				if (combo[index])
					occ |= (ONE_BB << (square + pairs.directions[index] * combo[index]));

				// Build each ray.
				for (int iter = 1; combo[index] > 0; combo[index]--, iter++)
					reach |= (ONE_BB << (square + iter * pairs.directions[index]));
			}

			reachSet[PIECE[square][getReachIndex(occ & mask[square], square)]] = reach;
		}
	}
	return reaches;
}

BitboardFamily computeRookReaches() {
	return computeReaches(computeRookReachIndices, rookReachRaySizes, getRookReachIndex, Masks::ROOK);
}

BitboardFamily computeBishopReaches() {
	return computeReaches(computeBishopReachIndices, bishopReachRaySizes, getBishopReachIndex, Masks::BISHOP);
}

std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> computeKingReachSquares() {
	std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> kingReaches;
	for (Square square = A1; square <= H8; square++) {
		std::vector<std::vector<Square>>& reachSet = kingReaches[square];
		std::vector<Square> destinations;
		if (square / RANK_COUNT != RANK_8) destinations.push_back(square + N);
		if (square % FILE_COUNT != FILE_H) destinations.push_back(square + E);
		if (square / RANK_COUNT != RANK_1) destinations.push_back(square + S);
		if (square % FILE_COUNT != FILE_A) destinations.push_back(square + W);
		if (square / RANK_COUNT != RANK_8 && square % FILE_COUNT != FILE_H) destinations.push_back(square + NE);
		if (square / RANK_COUNT != RANK_1 && square % FILE_COUNT != FILE_H) destinations.push_back(square + SE);
		if (square / RANK_COUNT != RANK_1 && square % FILE_COUNT != FILE_A) destinations.push_back(square + SW);
		if (square / RANK_COUNT != RANK_8 && square % FILE_COUNT != FILE_A) destinations.push_back(square + NW);

		uint16_t maxOccupancy = static_cast<uint16_t>(pow(2, destinations.size()));
		reachSet.resize(maxOccupancy);

		// Iterate all occupancies.
		for (uint16_t num = 0; num < maxOccupancy; num++) {
			std::vector<Square> reach;
			uint64_t occ = ZERO_BB;
			int shift = 0;

			// Build occupancy bitboard
			for (Square dest : destinations) {
				if (isSet<uint16_t>(num, shift))
					reach.push_back(dest);
				occ |= (extractBit<uint16_t>(num, shift) << dest);
				shift++;
			}

			uint16_t magicIndex = getKingMovesIndex(occ, square);
			reachSet[magicIndex] = reach;
		}
	}
	return kingReaches;
}

BitboardFamily computeLevelRays() {
	BitboardFamily rayFamilies;
	for (Square king = A1; king <= H8; king++) {
		BitboardVector rays;
		for (Square piece = A1; piece <= H8; piece++) {
			Bitboard ray = ZERO_BB;
			if (file(king) == file(piece)) {
				for (int sq = piece; sq != king;  king > piece ? sq += RANK_COUNT : sq -= RANK_COUNT)
					ray |= ONE_BB << sq;
			} else if (rank(king) == rank(piece)) {
				for (int sq = piece; sq != king;  king > piece ? sq++ : sq--)
					ray |= ONE_BB << sq;
			}
			rays.push_back(ray);
		}
		rayFamilies[king] = rays;
	}
	return rayFamilies;
}

BitboardFamily computeDiagonalRays() {
	BitboardFamily rayFamilies;
	for (Square king = A1; king <= H8; king++) {
		BitboardVector rays;
		for (Square piece = A1; piece <= H8; piece++) {
			Bitboard ray = ZERO_BB;
			if (std::abs(rank(king) - rank(piece)) == std::abs(file(king) - file(piece)) && king != piece) {
				int inc = king > piece ? (file(king) > file(piece) ? NE : NW) : (file(king) > file(piece) ? SE : SW);
				for (int i = piece; i != king; i += inc)
					ray |= ONE_BB << i;
			}
			rays.push_back(ray);
		}
		rayFamilies[king] = rays;
	}
	return rayFamilies;
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
	if (N == M)
		return n + m * N;
	else if (N < M)
		return m + n * M;
	else
		return n + m * N;
}

void generateCombination(ArrayFour sizes, ArrayFour curr, std::vector<ArrayFour>& res) {
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

std::vector<ArrayFour> getEndCombinations(ArrayFour sizes) {
	std::vector<ArrayFour> res;
	generateCombination(sizes, {0, 0, 0, 0}, res);
	return res;
}

void generateLoneSquares(ArrayFour sizes, std::vector<ArrayFour>& res) {
	for (uint i = 0; i < sizes.size(); i++) {
		for (int j = 0; j <= sizes[i]; j++) {
			ArrayFour combo = {0, 0, 0, 0};
			combo[i] = j;
			if (std::find(res.begin(), res.end(), combo) == res.end())
				res.push_back(combo);
		}
	}
}

void generatePairSquares(ArrayFour sizes, ArrayFour curr, std::vector<ArrayFour>& res) {
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
		ArrayFour combo = {0, 0, 0, 0};
		combo[first] = i;
		combo[second] = i;
		if (std::find(res.begin(), res.end(), combo) == res.end())
			res.push_back(combo);
	}
}

std::vector<ArrayFour> getEndBlockSquares(ArrayFour sizes) {
	std::vector<ArrayFour> res;
	generateLoneSquares(sizes, res);
	generatePairSquares({sizes[0], sizes[1], 0, 0}, {0, 0, 0, 0}, res);
	generatePairSquares({0, sizes[1], sizes[2], 0}, {0, 0, 0, 0}, res);
	generatePairSquares({0, 0, sizes[2], sizes[3]}, {0, 0, 0, 0}, res);
	generatePairSquares({sizes[0], 0, 0, sizes[3]}, {0, 0, 0, 0}, res);
	return res;
}