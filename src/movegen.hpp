
#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "constants.hpp"

struct Computed {
    MovesStruct BISHOP_BLOCKS[64];

    /////////////////////////////////////////

    MovesStruct KNIGHT_MOVES[64];

    /////////////////////////////////////////

    MovesStruct KING_MOVES[64];

    /////////////////////////////////////////

    MovesStruct PAWN_MOVES[2][48];

    /////////////////////////////////////////

    MovesStruct CASTLING_MOVES[4];

    /////////////////////////////////////////

    MovesStruct EN_PASSANT_MOVES[16];

    /////////////////////////////////////////

    MovesStruct DOUBLE_PUSH[16];
};

std::vector<std::vector<int>> computeRookIndices();
std::vector<std::vector<int>> computeBishopIndices();

typedef uint64_t Bitboard;
// typedef std::vector<Bitboard> BitBoardSet;
// typedef std::vector<BitBoardSet> BitBoardFamily;

// typedef uint16_t Move;
// typedef std::vector<Move> MoveSet;
// typedef std::vector<MoveSet> MoveFamily;
// typedef std::vector<MoveFamily> PieceMoves;

// typedef std::vector<std::unordered_map<Bitboard, std::vector<Move>>> CheckMap; // For check moves

namespace Compute {
    void init(Computed* moves);
}

/**
 * Calculates the index into the move_set of a particular move family.
 * 
 * @param masked_reach: A bitboard of the reach of the piece with own pieces
 *      masked out.
 * @param move_family: A pointer to Moves struct holding the move family. The
 *      block_bits vector MUST hold the squares in order from smallest to 
 *      largest.
 * @return:  Index into the move_set.
 */
int moveSetIndex(uint64_t masked_reach, MovesStruct* move_family);

/**
 * Finds and returns the position of the most significant set bit. If no bits
 * are set, returns 0.
 * 
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
 * Calls all functions to make the precomputations for a game.
 * @param moves: Pointer to precomputed moves structs.
 */
void precompute(Computed* moves);

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
void computeKingMoves(MovesStruct* KING_MOVES);

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
void computePawnMoves(MovesStruct PAWN_MOVES[][48], MovesStruct EN_PASSANT_MOVES[16], MovesStruct DOUBLE_PUSH[16]);

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
