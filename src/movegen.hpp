#pragma once

#ifdef USE_PEXT
#include <x86intrin.h>
#endif

#include <unordered_map>
#include <vector>

#include "constants.hpp"
#include "bitboard.hpp"
#include "utils.hpp"

/**
 * @brief Compute move sets of the king on each square based on legal destinations. This does not compute castling
 * moves.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeKingMoves();

/**
 * @brief Compute move sets of the rook on each square based on legal destinations.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeRookMoves();

/**
 * @brief Compute moves sets of the bishop on each square based on legal destinations.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeBishopMoves();

/**
 * @brief Compute move sets of the knight on each square based on legal destinations.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeKnightMoves();

/**
 * @brief Makes calls to computePawnMovesBySide to create the pawn moves arrays. Black array is 0, white array is 1.
 *
 * @return Array of array of vectors of move vectors.
 */
std::array<MoveFamilies, PLAYER_COUNT> computePawnMoves();

/**
 * @brief Compute move sets of the pawn on each square based of legal destinations for a particular side. Does not
 * compute en-passant moves.
 *
 * @param player The side to compute move sets for.
 * @return Array of vectors of move vectors.
 */
MoveFamilies computePawnMovesBySide(const Player player);

/**
 * @brief Compute the castling moves for both sides. Black array is 0, white array is 1.
 *
 * @return Array of move vectors.
 */
std::array<std::vector<Move>, CASTLING_OPTIONS> computeCastlingMoves();

/**
 * @brief Compute the en-passant moves.
 *
 * @return Some overly confusing data structure containing the en-passant moves.
 */
std::array<std::vector<std::vector<std::vector<Move>>>, PLAYER_COUNT> computeEnPassantMoves();

/**
 * @brief Compute the move sets for rook moves on each square based on legal destinations when the own side is in
 * check.
 *
 * @return Array of vector of move vectors.
 */
MoveFamilies computeRookBlockMoves();

/**
 * @brief Compute the move sets for bishop moves on each square based on legal destinations when the own side is in
 * check.
 *
 * @return Array of vector of move vectors.
 */
MoveFamilies computeBishopBlockMoves();

/**
 * @brief Compute the rook indices into the reach index array.
 *
 * @return Array of vectors of uints of the rook index into the precomputed reach bitboards.
 */
IndicesFamily computeRookReachIndices();

/**
 * @brief Compute the bishop indices into the reach index array.
 *
 * @return Array of vectors of uints of the bishop index into the precomputed reach bitboards.
 */
IndicesFamily computeBishopReachIndices();

/**
 * @brief Compute the reach bitboards for the different occupancies of the rook on each square.
 *
 * @return Array of vectors of reaches bitboards.
 */
BitboardFamily computeRookReaches();

/**
 * @brief Compute the reach bitboards for the different occupancies of the bishop on each square.
 *
 * @return Array of vectors of reaches bitboards.
 */
BitboardFamily computeBishopReaches();

/**
 * @brief Compute the squares of the reach of the king for different occupancies on each square.
 *
 * @return Squares of the reach of the king for different occupancies on each square.
 */
std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> computeKingReachSquares();

/**
 * @brief Compute the level rays from first square (exclusive) to second square (inclusive).
 *
 * @return Array of vector of reach bitboards.
 */
BitboardFamily computeLevelRays();

/**
 * @brief Compute the diagonal rays from first square (exclusive) to second square (inclusive).
 *
 * @return Array of vector of reach bitboards.
 */
BitboardFamily computeDiagonalRays();

/**
 * @brief Get the rook index into the precomputed reach index array.
 *
 * @param occupancy Occupancy bitboard.
 * @param square Square the rook is on.
 * @return Index into the precomputed rook reach array.
 */
inline uint getRookReachIndex(const Bitboard occupancy, const Square square) {
	#ifdef USE_PEXT
	return _pext_u64(occupancy, Masks::ROOK[square]);
	#else
	return (occupancy * MagicNums::Reach::ROOK[square]) >> Shifts::Reach::ROOK[square];
	#endif
}

/**
 * @brief Get the rook index into the precomputed moves index array.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the rook is on.
 * @return Index into the precomputed rook moves array.
 */
inline uint getRookMovesIndex(const Bitboard reach, const Square square) {
	return (reach * MagicNums::Moves::ROOK[square]) >> Shifts::Moves::ROOK[square];
}

/**
 * @brief Get the bishop index into the precomputed reach index arrays.
 *
 * @param occupancy Occupancy bits of concern.
 * @param square Square the bishop of concern is on.
 * @return Index into the precomputed bishop reach array.
 */
inline uint getBishopReachIndex(const Bitboard occupancy, const Square square) {
	#ifdef USE_PEXT
	return _pext_u64(occupancy, Masks::BISHOP[square]);
	#else
	return (occupancy * MagicNums::Reach::BISHOP[square]) >> Shifts::Reach::BISHOP[square];
	#endif
}

/**
 * @brief Get the bishop index into the precomputed moves index arrays.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the bishop of concern is on.
 * @return Index into the precomputed bishop moves array.
 */
inline uint getBishopMovesIndex(const Bitboard reach, const Square square) {
	return (reach * MagicNums::Moves::BISHOP[square]) >> Shifts::Moves::BISHOP[square];
}

/**
 * @brief Get the knight index into the precomputed moves arrays.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the knight of concern is on.
 * @return Index into the precomputed knight moves array.
 */
inline uint getKnightMovesIndex(const Bitboard reach, const Square square) {
	#ifdef USE_PEXT
	return _pext_u64(reach, Masks::KNIGHT[square]);
	#else
	return (reach * MagicNums::KNIGHT[square]) >> Shifts::KNIGHT[square];
	#endif
}

/**
 * @brief Get the king index into the precomputed moves arrays.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the king of concern is on.
 * @return Index into the precomputed king moves array.
 */
inline uint getKingMovesIndex(const Bitboard reach, const Square square) {
	#ifdef USE_PEXT
	return _pext_u64(reach, Masks::KING[square]);
	#else
	return (reach * MagicNums::KING[square]) >> Shifts::KING[square];
	#endif
}

/**
 * @brief Get the pawn index into the precomputed moves arrays.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the pawn of concern is on.
 * @param player Which sides pawn.
 * @return Index into the precomputed pawn moves array.
 */
inline uint getPawnMovesIndex(const Bitboard reach, const Square square, const Player player) {
	#ifdef USE_PEXT
	return _pext_u64(reach, Masks::PAWN[player][square]);
	#else
	return (reach * MagicNums::PAWN[player][square]) >> Shifts::PAWN[player][square];
	#endif
}

/**
 * @brief Get the rook index into the precomputed block moves arrays.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the rook of concern is on.
 * @return Index into the precomputed rook block moves array.
 */
inline uint getRookBlockIndex(const Bitboard reach, const Square square) {
	return (reach * MagicNums::Block::ROOK[square]) >> Shifts::Block::ROOK[square];
}

/**
 * @brief Get the bishop index into the precomputed block moves arrays.
 *
 * @param reach Reach of the piece. Each set bit is a legal destination square.
 * @param square Square the bishop of concern is on.
 * @return Index into the precomputed bishop block moves array.
 */
inline uint getBishopBlockIndex(const Bitboard reach, const Square square) {
	return (reach * MagicNums::Block::BISHOP[square]) >> Shifts::Block::BISHOP[square];
}

/**
 * @brief Hash the ordered integers in "values" to a unique index.
 *
 * @param values Ordered collection of integers.
 * @param ranges The number ranges[i] is the cardinality of the set from which values[i] comes from.
 * @return A unique index mapping based on a composition of the function pairingFunction.
 */
int getIndex(const std::vector<int> values, const std::vector<int> ranges);

/**
 * @brief Maps ordered pairs of integers (n, m) to a unique int in the set {0, 1, ..., N * M - 1}.
 *
 * @param n A non-negative integer in the set {0, 1, ..., N - 1}.
 * @param m A non-negative integer in the set {0, 1, ..., M - 1}.
 * @param N Cardinality of the set of possible values for n.
 * @param M Cardinality of the set of possible values for m.
 * @return A unique integer from the set {0, 1, ..., N * M - 1} for each ordered pair (n, m).
 */
int pairingFunction(const int n, const int m, const int N, const int M);

/**
 * @brief Adds the curr combination to the res vector and increments curr[0]. If an overflow occurs, it resolves the
 * overflow by doing the necessary increments to the remaining array and performs a recursive call on the new updated
 * curr array. Recursion stops when curr == sizes; The values in sizes and curr is ordered N, E, S, W for rooks and
 * NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible position of the first blocking bit.
 * @param curr The current array combination.
 * @param res Vector of arrays to store the results.
 */
void generateCombination(std::array<int, 4> sizes, std::array<int, 4> curr, std::vector<std::array<int, 4>>& res);

/**
 * @brief Calls generateCombination and returns a vector of arrays of all different end combinations depending on values
 * in the sizes array. The values in sizes is ordered N, E, S, W for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible position of the first blocking bit.
 * @return Vector of arrays of possible end combinations that are no larger than the corresponding size.
 */
std::vector<std::array<int, 4>> getEndCombinations(std::array<int, 4> sizes);

/**
 * @brief Generates the lone square moves for a rook when its own side is in check. The values in sizes is ordered N, E,
 * S, W for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible positions of the blocking bit.
 * @param res Vector of arrays to store the results.
 */
void generateLoneSquares(std::array<int, 4> sizes, std::vector<std::array<int, 4>>& res);

/**
 * @brief Generates the square move pairs for a rook when its own side is in check. The values in sizes is ordered N, E,
 * S, W for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible positions of the blocking bit.
 * @param res Vector of arrays to store the results.
 */
void generatePairSquares(std::array<int, 4> sizes, std::array<int, 4> curr, std::vector<std::array<int, 4>>& res);

/**
 * @brief Generates the blocking square combinations based on the sizes array. The values in sizes is ordered N, E, S, W
 * for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible positions of the blocking bit.
 * @return std::vector<std::array<int, 4>>
 */
std::vector<std::array<int, 4>> getEndBlockSquares(std::array<int, 4> sizes);

/**
 * Information before current move is made. In the Position class, the 'history' contains structs of type History. The
 * last entry is described by this struct.
 */
struct History {
	uint castling;      // Castling permissions immediately before the current position.
	Square enPassant;  // En passant permission immediately before the current position.
	uint halfmove;      // Halfmove count immediately before the current position.
	uint64_t hash;      // The hash of immediately before the current position.
	Move move;          // The move that resulted in the current position.
	PieceType captured; // The piece captured just before the current position (if any).
};