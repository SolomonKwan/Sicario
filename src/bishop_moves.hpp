
#ifndef BISHOP_MOVES_HPP
#define BISHOP_MOVES_HPP

#include "constants.hpp"

/**
 * Returns the index from BISHOP_INDEX into BISHOP_MOVES based on the square and 
 * occupancy.
 * 
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 */
const int bishopIndex(const uint64_t pos, Square square);

/**
 * Gets a bitboard of the rays between the bishop (queen) checkers and the king
 * being checked.
 * 
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t getBishopCheckRays(Game* game, Computed* moves, Square square,
        uint64_t* checkers_only);

/**
 * Iterates through the squares and call functions to compute bishop moves and
 * calculate indices.
 * 
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of bishop move sets to store precomputed moves.
 */
void computeBishopMoves(std::vector<int>* BISHOP_INDEX, Moves* BISHOP_MOVES);

/**
 * Computes the blocking moves (and captures in case of check) for bishops on 
 * each square.
 * 
 * @param BISHOP_BLOCKS: A vector of move structs.
 */
void computeBishopBlocks(Moves* BISHOP_BLOCKS);

/**
 * Finds and returns a pointer to a bishop move family.
 * 
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the bishop is on.
 */
Moves* getBishopFamily(Game* game, Computed* moves, Square square);

#endif
