
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
 */
std::array<std::vector<Move>, 64> generateKingMoves();

/**
 * Compute pseudolegal moves of the knight on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account if the knight is pinned.
 */
std::array<std::vector<Move>, 64> generateKnightMoves();

/**
 * Compute pseudolegal moves of the pawn on each square.
 * @return: Array of array of vectors of pseudolegal moves.
 * @note: Does not take into account if the pawn is pinned.
 * @note: Black array is 0, white array is 1.
 */
std::array<std::array<std::vector<Move>, 64>,2> generatePawnMoves();

/**
 * Compute pseudolegal moves of the rook on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account if the rook is pinned.
 */
std::array<std::vector<std::vector<Move>>, 64> generateRookMoves();

/**
 * Compute pseudolegal moves of the bishop on each square.
 * @return: Array of vectors of pseudolegal moves.
 * @note: Does not take into account if the bishop is pinned.
 */
std::array<std::vector<std::vector<Move>>, 64> generateBishopMoves();

/**
 * Compute the rook indices into the move set for the magic indices.
 * @return: Array of vectors of ints of the rook index into the move set.
 */
std::array<std::vector<int>, 64> generateRookIndices();

/**
 * Compute the bishop indices into the move set for the magic indices.
 * @return: Array of vectors of ints of the bishop index into the move set.
 */
std::array<std::vector<int>, 64> generateBishopIndices();

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

std::vector<std::vector<int>> computeRookIndices();
std::vector<std::vector<int>> computeBishopIndices();
std::vector<MovesStruct> computeEnPassantMoves();
std::vector<MovesStruct> computeDoublePushMoves();
std::vector<std::vector<Bitboard>> computeLevelRays();
std::vector<std::vector<Bitboard>> computeDiagonalRays();

/**
 * Calculates the index into the move_set of a particular move family.
 *
 * @param masked_reach: A bitboard of the reach of the piece with own pieces masked out.
 * @param move_family: A pointer to Moves struct holding the move family. The block_bits vector MUST hold the squares
 *      in order from smallest to largest.
 * @return:  Index into the move_set.
 */
int moveSetIndex(uint64_t masked_reach, MovesStruct* move_family);

/**
 * Finds and returns the position of the most significant set bit. If no bits are set, returns 0.
 * @param n: The integer to find the MSB.
 * @return: The most significant bit position.
 */
int MSB(int n);

/**
 * Returns true if the nths bit of position is set, else false.
 *
 * @param position: A bitboard.
 * @param n: The wanted bit.
 * @return: True if bit is set, else false.
 */
bool bitAt(uint64_t position, int n);

/**
 * Returns the index from BISHOP_INDEX into BISHOP_MOVES based on the square and
 * occupancy.
 *
 * @param position: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 */
const int bishopIndex(const uint64_t position, Square square);

/**
 * Iterates through the squares and call functions to compute bishop moves and
 * calculate indices.
 *
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of bishop move sets to store precomputed moves.
 */
std::vector<MovesStruct> computeBishopMoves();

/**
 * Computes the blocking moves (and captures in case of check) for bishops on
 * each square.
 *
 * @param BISHOP_BLOCKS: A vector of move structs.
 */
std::vector<MovesStruct> computeBishopBlocks();

/**
 * Compute the king moves.
 * @param KING_MOVES: The array of king moves to be computed.
 */
std::vector<MovesStruct> computeKingMoves();

std::vector<MovesStruct> computeCastling();

/**
 * Compute the knight moves.
 * @param KNIGHT_MOVES: Array of knight moves to be computed.
 */
std::vector<MovesStruct> computeKnightMoves();

/**
 * Computes the pawn moves.
 * @param PAWN_MOVES: A 2d array of moves structs for pawns.
 * @param EN_PASSANT_MOVES: Array of en-passant moves structs. 32 is the offset
 *  to index a moves struct. move_set index of 1 is for left ep, 0 is for right
 *  ep.
 * @param DOUBLE_PUSH: An array of moves structs for double pawn push blocks.
 */
std::vector<std::vector<MovesStruct>> computePawnMoves();

/**
 * Returns the index from ROOK_INDEX into ROOK_MOVES based on the square and
 * occupancy.
 * @param position: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 * @return: Index into ROOK_MOVES.
 */
const int rookIndex(const uint64_t position, Square square);

/**
 * Iterates through the squares and call functions to compute rook moves and
 * calculate indices.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Move struct to store the moves.
 */
std::vector<MovesStruct> computeRookMoves();

/**
 * Computes the blocking moves (and captures in case of check) for rooks on each
 * square.
 */
std::vector<MovesStruct> computeRookBlocks();

#endif
