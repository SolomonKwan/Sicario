
#ifndef ROOK_MOVES_HPP
#define ROOK_MOVES_HPP

#include "movegen.hpp"

/**
 * Returns the index from ROOK_INDEX into ROOK_MOVES based on the square and 
 * occupancy.
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 * @return: Index into ROOK_MOVES.
 */
const int rookIndex(const uint64_t pos, Square square);

/**
 * Gets a bitboard of the rays between the rook (queen) checkers and the king
 * being checked.
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers: Bitboard of checkers.
 * @return: Check rays of rooks and queens.
 */
uint64_t getRookCheckRays(Game* game, Computed* moves, Square square, 
        uint64_t* checkers);

/**
 * Iterates through the squares and call functions to compute rook moves and
 * calculate indices.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Move struct to store the moves.
 */
void computeRookMoves(std::vector<int>* ROOK_INDEX, MovesStruct* ROOK_MOVES);

/**
 * Computes the blocking moves (and captures in case of check) for rooks on each
 * square.
 * @param ROOK_BLOCKS: A vector of move structs.
 */
void computeRookBlocks(MovesStruct* ROOK_BLOCKS);

/**
 * Finds and returns a pointer to a rook move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* getRookFamily(Game* game, Computed* moves, Square square);

/**
 * Finds and returns a pointer to a rook block move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* getRookBlockFamily(Game* game, Computed* moves, Square square);

#endif
