
#ifndef GAME_HPP
#define GAME_HPP

#include "constants.hpp"

namespace Play {
    void init(Pos* game, Computed* moves, CmdLine* args,char *argv[], 
            std::string input);
}

/**
 * Checks if end of game has been reached.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves.
 * @param enemy_attacks: Bitboard of squares attacked by enemy pieces.
 * @param moves_index: Pointer to number of vectors in pos_moves.
 * @return: The appropriate ExitCode.
 */
ExitCode isEOG(Pos* game, Computed* moves, uint64_t enemy_attacks, 
        int move_index);

/**
 * Show the usage of the program.
 * 
 * @param argv: Array of command line arguments.
 */
void showUsage(char *argv[]);

/**
 * Show the end of game message.
 * @param code: The exitcode of the game.
 * @param argv: Array of command line arguments.
 */
void showEOG(ExitCode code, char *argv[]);

/**
 * Handles a single game.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param args: Pointer to command line arguments struct.
 */
void handleGame(Pos* game, Computed* Moves, CmdLine* args, char *argv[]);

/**
 * Make the moves.
 * @param move: The moves to make.
 * @param game: Pointer to game struct.
 */
void makeMove(uint16_t move, Pos* game);

/**
 * Undo moves.
 * @param game: Pointer to game struct.
 */
void undoMove(Pos* game);

/**
 * Retrives all legal moves of the current position.
 * 
 * @param game: Pointer to Pos struct of the current position.
 * @param moves: Pointer to the precomputed moves struct.
 * @param enemy_attacks: Pointer to bitboard of all squares attacked by the 
 *  enemy.
 * 
 * @return: The number of move sets.
 */
int getMoves(Pos* game, Computed* moves, uint64_t* enemy_attacks, 
        std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS]);

#endif
