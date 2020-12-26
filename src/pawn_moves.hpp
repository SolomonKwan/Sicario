
#ifndef PAWN_MOVES_HPP
#define PAWN_MOVES_HPP

#include "movegen.hpp"

/**
 * Computes the pawn moves.
 * @param PAWN_MOVES: A 2d array of moves structs for pawns.
 * @param EN_PASSANT_MOVES: Array of en-passant moves structs. 32 is the offset
 *  to index a moves struct. move_set index of 1 is for left ep, 0 is for right
 *  ep.
 * @param DOUBLE_PUSH: An array of moves structs for double pawn push blocks.
 */
void computePawnMoves(Moves PAWN_MOVES[][48], Moves EN_PASSANT_MOVES[16], 
        Moves DOUBLE_PUSH[16]);

/**
 * Get the arguments for the pawn index function for pawns.
 * @param game: Pointer to game struct.
 * @param square: Square of pawn.
 */
uint64_t pawnMoveArgs(Game* game, Square square);

/**
 * Returns a bitboard of all pawn checkers.
 * @param game: The game struct pointer of the current position.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t getPawnCheckers(Game* game, Square square, uint64_t* checkers_only);

#endif
