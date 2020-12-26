
#ifndef KNIGHT_MOVES_HPP
#define KNIGHT_MOVES_HPP

#include "movegen.hpp"

/**
 * Compute the knight moves.
 * @param KNIGHT_MOVES: Array of knight moves to be computed.
 */
void computeKnightMoves(Moves* KNIGHT_MOVES);

/**
 * Gets a bitboard of knight checkers.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the king in check.
 * @param checkers_only: A pointer to a bit board of checkers only.
 * @return: Bitboard of knight checkers.
 */
uint64_t getKnightCheckers(Game* game, Computed* move, Square square, 
        uint64_t* checkers_only);

#endif
