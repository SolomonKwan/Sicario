
#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include <vector>
#include <unordered_map>
#include "constants.hpp"

typedef uint16_t Move;
typedef uint64_t Bitboard;

enum Player {
    BLACK, WHITE
};

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
 * Returns true if the nths bit of pos is set, else false.
 * 
 * @param pos: A bitboard.
 * @param n: The wanted bit.
 * @return: True if bit is set, else false.
 */
bool bitAt(uint64_t pos, int n);

/**
 * @param pos: A bitboard.
 */
void displayBB(uint64_t pos);

/**
 * Returns the index from BISHOP_INDEX into BISHOP_MOVES based on the square and 
 * occupancy.
 * 
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 */
const int bishopIndex(const uint64_t pos, Square square);

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
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 * @return: Index into ROOK_MOVES.
 */
const int rookIndex(const uint64_t pos, Square square);

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
