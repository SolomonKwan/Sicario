
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

/**
 * Compute the king moves.
 * @param KING_MOVES: The array of king moves to be computed.
 */
void computeKingMoves(Moves* KING_MOVES);

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
void computeRookMoves(std::vector<int>* ROOK_INDEX, Moves* ROOK_MOVES);

/**
 * Computes the blocking moves (and captures in case of check) for rooks on each
 * square.
 * @param ROOK_BLOCKS: A vector of move structs.
 */
void computeRookBlocks(Moves* ROOK_BLOCKS);

/**
 * Finds and returns a pointer to a rook move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
Moves* getRookFamily(Game* game, Computed* moves, Square square);

/**
 * Finds and returns a pointer to a rook block move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
Moves* getRookBlockFamily(Game* game, Computed* moves, Square square);

#endif
