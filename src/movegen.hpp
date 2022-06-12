
#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include <unordered_map>
#include <vector>
#include "constants.hpp"

#define UNSET 0xFFFFFFFFFFFFFFFF // CHECK

enum Direction {
    N = 8,
    NE = 9,
    E = 1,
    SE = -7,
    S = -8,
    SW = -9,
    W = -1,
    NW = 7
};

enum KnightDirection {
    NNE = +17,
    ENE = +10,
    ESE = -6,
    SES = -15,
    SWS = -17,
    WSW = -10,
    WNW = +6,
    NWN = +15,
};

/**
 * Compute pseudolegal moves of the king on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account king potentially moving into check.
 * @note: Does not take into account king potentially taking own pieces.
 */
std::array<std::vector<Move>, 64> computeKingMoves();

/**
 * Compute pseudolegal moves of the knight on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account if the knight is pinned.
 * @note: Does not take into account if knight takes own pieces.
 */
std::array<std::vector<Move>, 64> computeKnightMoves();

/**
 * Compute pseudolegal moves of the pawn on each square.
 * @return: Array of array of vectors of pseudolegal moves.
 * @note: Does not take into account if the pawn is pinned.
 * @note: Does not take into account capturing own pieces.
 * @note: Black array is 0, white array is 1.
 */
std::array<std::array<std::vector<Move>, 64>,2> computePawnMoves();

/**
 * Compute pseudolegal moves of the rook on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account if the rook is pinned.
 * @note: Does not take into account if rook captures own pieces.
 */
std::array<std::vector<std::vector<Move>>, 64> computeRookMoves();

/**
 * Compute pseudolegal moves of the bishop on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account if the bishop is pinned.
 * @note: Does not take into account if bishop captures own pieces.
 */
std::array<std::vector<std::vector<Move>>, 64> computeBishopMoves();

/**
 * Compute the rook indices into the move set for the magic indices.
 * @return: Array of vectors of ints of the rook index into the move set.
 */
std::array<std::vector<int>, 64> computeRookIndices();

/**
 * Compute the bishop indices into the move set for the magic indices.
 * @return: Array of vectors of ints of the bishop index into the move set.
 */
std::array<std::vector<int>, 64> computeBishopIndices();

/**
 * @brief Gets the rook index into the precomputed reach arrays.
 * @param occupancy The occupancy bits of concern.
 * @param square The square the rook of concern is on.
 * @return Index into the precomputed rook reach array.
 */
inline int getRookReachIndex(Bitboard occupancy, Square square);

/**
 * @brief Gets the rook index into the precomputed moves arrays.
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the rook of concern is on.
 * @return Index into the precomputed rook moves array.
 */
inline int getRookMovesIndex(Bitboard reach, Square square);

/**
 * @brief Gets the bishop index into the precomputed reach arrays.
 * @param occupancy The occupancy bits of concern.
 * @param square The square the bishop of concern is on.
 * @return Index into the precomputed bishop reach array.
 */
inline int getBishopReachIndex(Bitboard occupancy, Square square);

/**
 * @brief Gets the bishop index into the precomputed moves arrays.
 * @param reach The reach of the piece. Each set bit is a legal destination square.
 * @param square The square the bishop of concern is on.
 * @return Index into the precomputed bishop moves array.
 */
inline int getBishopMovesIndex(Bitboard reach, Square square);

/**
 * Hash the ordered integers in values to a unique index.
 * @param values: Ordered collection of integers.
 * @param ranges: The number ranges[i] is the cardinality of the set from which values[i] comes from.
 * @return: A unique index mapping based on a composition of the function pairingFunction.
 */
int getIndex(std::vector<int> values, std::vector<int> ranges);

/**
 * Maps ordered pairs of integers (n, m) to a unique int in the set {0, 1, ..., N * M - 1}.
 * @param n: A non-negative integer in the set {0, 1, ..., N - 1}.
 * @param m: A non-negative integer in the set {0, 1, ..., M - 1}.
 * @param N: Cardinality of the set of possible values for n.
 * @param M: Cardinality of the set of possible values for m.
 * @return: A unique integer from the set {0, 1, ..., N * M - 1} for each ordered pair (n, m).
 */
int pairingFunction(int n, int m, int N, int M);

/**
 * A struct holding the move families.
 *
 * Contains:
 *  reach:      A bit board of the squares a piece can reach. Enemy and friendly
 *              pieces included. Goes all the way to the edge of the board.
 *  block_bits: A vector of the end squares on each ray of the reach bitboard.
 *  move_set:   A vector of vectors of 16 bit unsigned integers. Each integer
 *              encodes a move as Promotion (4) | MoveType (4) | Destination (6)
 *              | Origin (6).
 *  en_passant: Vector of vectors of en-passant moves. Used only for pawns.
 *  checked_moves: Unnorderd map whose keys are uint64_t ints with the possible
 *      destination squares (captures and blocks) set. The value is a vector of
 *      moves to those squares.
 */
struct MovesStruct {
    uint64_t reach;
    std::vector<int> block_bits;
    std::vector<std::vector<Move>> move_set;
    std::vector<std::vector<Move>> en_passant;
    std::vector<std::vector<Move>> double_push;

    std::unordered_map<uint64_t, std::vector<Move>> checked_moves;
};

/**
 * Information before current move is made.
 */
struct History {
    int castling;
    Square en_passant;
    int halfmove;
    Move move;
    PieceType captured;
    uint64_t hash;
};

#endif
