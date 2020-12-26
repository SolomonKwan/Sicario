
#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "constants.hpp"

namespace Compute {
    void init(Computed* moves);
}

/**
 * Prints a readable version of a move.
 * @param move: The move to be printed.
 */
void printMove(uint16_t move, bool extraInfo = false);

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
int moveSetIndex(uint64_t masked_reach, Moves* move_family);

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
 * Displays the bitboard.
 * @param game: A pointer to the game struct.
 * @param args: The command line arguments.
 */
void display(Game* game, CmdLine* args);

/**
 * @param pos: A bitboard.
 */
void displayBB(uint64_t pos);

/**
 * Display all individual game position information.
 * @param game: Pointer to game struct.
 */
void displayAll(Game* game);

/**
 * Calls all functions to make the precomputations for a game.
 * @param moves: Pointer to precomputed moves structs.
 */
void precompute(Computed* moves);

/**
 * Sets all the legal moves a king can make in a given position.
 * @param game: Pointer to game struct.
 * @param computed_moves: Pointer to precomputed moves struct.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 * @param kEnemy_attacks: Bitboard of enemy attacks with king masked out.
 */
void getKingMoves(Game* game, Computed* computed_moves, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, 
        uint64_t kEnemy_attacks);

/**
 * Gets the squares attacked by an enemy piece.
 * 
 * @param game: A game struct pointer.
 * @param moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param kEnemy_attacks: 64 bit unsigned int pointer with value 0.
 */
void getEnemyAttacks(Game* game, Computed* moves, 
        uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, 
        uint64_t* kEnemy_attacks);

/**
 * Gets all legal moves for the player whose turn it is.
 * @param game: A game struct pointer.
 * @param computed_moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 * @param kEnemy_attacks: Bitboard of enemy attacks with king masked out.
 */
void getNormalMoves(Game* game, Computed* computed_moves, 
        uint64_t* enemy_attacks, uint64_t* rook_pins, uint64_t* bishop_pins, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, 
        uint64_t kEnemy_attacks);

/**
 * Retrieves the legal moves for when the king is in check.
 * @param game: A game struct pointer.
 * @param moves: Precomputed moves struct pointer.
 * @param enemy_attacks: 64 bit unsigned int pointer for computing king moves.
 * @param rook_pins: 64 bit unsigned int pointer for rook pins.
 * @param bishop_pins: 64 bit unsigned int pointer for bishop pins.
 * @param pos_moves: Array of 16 bit unsigned int move vectors.
 * @param moves_index: Pointer to number of move struct in pos_moves.
 * @param kEnemy_attacks: Bitboard of enemy attacks with king masked out.
 */
void getCheckedMoves(Game* game, Computed* moves, uint64_t* enemy_attacks, 
        uint64_t* rook_pins, uint64_t* bishop_pins, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS], int* moves_index, 
        uint64_t kEnemy_attacks);

#endif
