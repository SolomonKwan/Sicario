
#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#ifdef USE_PEXT
#include <x86intrin.h>
#endif

#include <unordered_map>
#include <vector>

#include "constants.hpp"
#include "bitboard.hpp"
#include "utils.hpp"

/**
 * @brief Computes move sets of the king on each square based on legal destinations. This does not compute castling
 * moves.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeKingMoves();

/**
 * @brief Computes move sets of the rook on each square based on legal destinations.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeRookMoves();

/**
 * @brief Computes moves sets of the bishop on each square based on legal destinations.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeBishopMoves();

/**
 * @brief Computes move sets of the knight on each square based on legal destinations.
 *
 * @return Array of vectors of move vectors.
 */
MoveFamilies computeKnightMoves();

/**
 * @brief Makes calls to computePawnMovesBySide to create the pawn moves arrays. Black array is 0, white array is 1.
 *
 * @return Array of array of vectors of move vectors.
 */
std::array<MoveFamilies,2> computePawnMoves();

/**
 * @brief Computes move sets of the pawn on each square based of legal destinations for a particular side. Does not
 * compute en-passant moves.
 *
 * @param player The side to compute move sets for.
 * @return: Array of vectors of move vectors.
 */
MoveFamilies computePawnMovesBySide(Player player);

/**
 * @brief Computed the castling moves for both sides. Black array is 0, white array is 1.
 *
 * @return Array of move vectors.
 */
std::array<std::vector<Move>, 4> computeCastlingMoves();

std::array<std::vector<std::vector<std::vector<Move>>>, 2> computeEnPassantMoves();

/**
 * @brief Computes the move sets for rook moves on each square based on legal destinations when the own side is in
 * check.
 *
 * @return Array of vector of move vectors.
 */
MoveFamilies computeRookBlockMoves();

/**
 * @brief Computes the move sets for bishop moves on each square based on legal destinations when the own side is in
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
 * @brief Computes the reach bitboards for the different occupancies of the bishop on each square.
 *
 * @return Array of vectors of reaches bitboards.
 */
BitboardFamily computeBishopReaches();

/**
 * @brief Computes the squares of the reach of the king for different occupancies on each square.
 *
 * @return Squares of the reach of the king for different occupancies on each square.
 */
std::array<std::vector<std::vector<Square>>, SQUARE_COUNT> computeKingReachSquares();

/**
 * @brief Computes the level rays from first square (exclusive) to second square (inclusive).
 *
 * @return Array of vector of reach bitboards.
 */
BitboardFamily computeLevelRays();

/**
 * @brief Computes the diagonal rays from first square (exclusive) to second square (inclusive).
 *
 * @return Array of vector of reach bitboards.
 */
BitboardFamily computeDiagonalRays();

/**
 * @brief Gets the rook index into the precomputed reach index array.
 *
 * @param occupancy The occupancy bits of concern.
 * @param square The square the rook of concern is on.
 * @return Index into the precomputed rook reach array.
 */
inline uint getRookReachIndex(Bitboard occupancy, Square square) {
    #ifdef USE_PEXT
    return _pext_u64(occupancy, Masks::ROOK[square]);
    #else
    return (occupancy * MagicNums::Reach::ROOK[square]) >> Shifts::Reach::ROOK[square];
    #endif
}

/**
 * @brief Gets the rook index into the precomputed moves index array.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the rook of concern is on.
 * @return Index into the precomputed rook moves array.
 */
inline uint getRookMovesIndex(Bitboard reach, Square square) {
    return (reach * MagicNums::Moves::ROOK[square]) >> Shifts::Moves::ROOK[square];
}

/**
 * @brief Gets the bishop index into the precomputed reach index arrays.
 *
 * @param occupancy The occupancy bits of concern.
 * @param square The square the bishop of concern is on.
 * @return Index into the precomputed bishop reach array.
 */
inline uint getBishopReachIndex(Bitboard occupancy, Square square) {
    #ifdef USE_PEXT
    return _pext_u64(occupancy, Masks::BISHOP[square]);
    #else
    return (occupancy * MagicNums::Reach::BISHOP[square]) >> Shifts::Reach::BISHOP[square];
    #endif
}

/**
 * @brief Gets the bishop index into the precomputed moves index arrays.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the bishop of concern is on.
 * @return Index into the precomputed bishop moves array.
 */
inline uint getBishopMovesIndex(Bitboard reach, Square square) {
    return (reach * MagicNums::Moves::BISHOP[square]) >> Shifts::Moves::BISHOP[square];
}

/**
 * @brief Gets the knight index into the precomputed moves arrays.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the knight of concern is on.
 * @return Index into the precomputed knight moves array.
 */
inline uint getKnightMovesIndex(Bitboard reach, Square square) {
    #ifdef USE_PEXT
    return _pext_u64(reach, Masks::KNIGHT[square]);
    #else
    return (reach * MagicNums::KNIGHT[square]) >> Shifts::KNIGHT[square];
    #endif
}

/**
 * @brief Gets the king index into the precomputed moves arrays.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the king of concern is on.
 * @return Index into the precomputed king moves array.
 */
inline uint getKingMovesIndex(Bitboard reach, Square square) {
    #ifdef USE_PEXT
    return _pext_u64(reach, Masks::KING[square]);
    #else
    return (reach * MagicNums::KING[square]) >> Shifts::KING[square];
    #endif
}

/**
 * @brief Gets the pawn index into the precomputed moves arrays.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the pawn of concern is on.
 * @param player Which sides pawn.
 * @return Index into the precomputed pawn moves array.
 */
inline uint getPawnMovesIndex(Bitboard reach, Square square, Player player) {
    #ifdef USE_PEXT
    return _pext_u64(reach, Masks::PAWN[player][square]);
    #else
    return (reach * MagicNums::PAWN[player][square]) >> Shifts::PAWN[player][square];
    #endif
}

/**
 * @brief Gets the rook index into the precomputed block moves arrays.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the rook of concern is on.
 * @return Index into the precomputed rook block moves array.
 */
inline uint getRookBlockIndex(Bitboard reach, Square square) {
    return (reach * MagicNums::Block::ROOK[square]) >> Shifts::Block::ROOK[square];
}

/**
 * @brief Gets the bishop index into the precomputed block moves arrays.
 *
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the bishop of concern is on.
 * @return Index into the precomputed bishop block moves array.
 */
inline uint getBishopBlockIndex(Bitboard reach, Square square) {
    return (reach * MagicNums::Block::BISHOP[square]) >> Shifts::Block::BISHOP[square];
}

/**
 * @brief Hash the ordered integers in "values" to a unique index.
 *
 * @param values Ordered collection of integers.
 * @param ranges The number ranges[i] is the cardinality of the set from which values[i] comes from.
 * @return A unique index mapping based on a composition of the function pairingFunction.
 */
int getIndex(std::vector<int> values, std::vector<int> ranges); // TODO Change to uint

/**
 * @brief Maps ordered pairs of integers (n, m) to a unique int in the set {0, 1, ..., N * M - 1}.
 *
 * @param n A non-negative integer in the set {0, 1, ..., N - 1}.
 * @param m A non-negative integer in the set {0, 1, ..., M - 1}.
 * @param N Cardinality of the set of possible values for n.
 * @param M Cardinality of the set of possible values for m.
 * @return A unique integer from the set {0, 1, ..., N * M - 1} for each ordered pair (n, m).
 */
int pairingFunction(int n, int m, int N, int M); // TODO change to uint

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
void generateCombination(std::array<int, 4> sizes, std::array<int, 4>& curr, std::vector<std::array<int, 4>>& res); // TODO change to uint

/**
 * @brief Calls generateCombination and returns a vector of arrays of all different end combinations depending on values
 * in the sizes array. The values in sizes is ordered N, E, S, W for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible position of the first blocking bit.
 * @return Vector of arrays of possible end combinations that are no larger than the corresponding size.
 */
std::vector<std::array<int, 4>> getEndCombinations(std::array<int, 4> sizes); // TODO change to uint

/**
 * @brief Generates the lone square moves for a rook when its own side is in check. The values in sizes is ordered N, E,
 * S, W for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible positions of the blocking bit.
 * @param res Vector of arrays to store the results.
 */
void generateLoneSquares(std::array<int, 4> sizes, std::vector<std::array<int, 4>>& res); // TODO change to uint

/**
 * @brief Generates the square move pairs for a rook when its own side is in check. The values in sizes is ordered N, E,
 * S, W for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible positions of the blocking bit.
 * @param res Vector of arrays to store the results.
 */
void generatePairSquares(std::array<int, 4> sizes, std::array<int, 4>& curr, std::vector<std::array<int, 4>>& res); // TODO change to uint

/**
 * @brief Generates the blocking square combinations based on the sizes array. The values in sizes is ordered N, E, S, W
 * for rooks and NE, SE, SW, NW for bishops.
 *
 * @param sizes Array of ints. Each int is the cardinality of the set of possible positions of the blocking bit.
 * @return std::vector<std::array<int, 4>>
 */
std::vector<std::array<int, 4>> getEndBlockSquares(std::array<int, 4> sizes); // TODO change to uint

/**
 * Information before current move is made.
 */
struct History {
    uint castling;
    Square en_passant;
    uint halfmove;
    Move move;
    PieceType captured;
    uint64_t hash;
};

#endif
