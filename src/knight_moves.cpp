
#include <algorithm>
#include <cmath>

#include "knight_moves.hpp"

/**
 * Compute the knight moves.
 * @param KNIGHT_MOVES: Array of knight moves to be computed.
 */
void Game::computeKnightMoves(Moves* KNIGHT_MOVES) {
    // Compute the moves and reach.
    for (int square = A1; square <= H8; square++) {
        // Compute the reach.
        KNIGHT_MOVES[square].reach = 0;
        
        if (square / 8 < 6 && square % 8 < 7) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 17); // NNE.
            KNIGHT_MOVES[square].block_bits.push_back(square + 17);
        }
        if (square / 8 < 7 && square % 8 < 6) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 10); // ENE.
            KNIGHT_MOVES[square].block_bits.push_back(square + 10);
        }
        if (square / 8 > 0 && square % 8 < 6) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 6); // ESE.
            KNIGHT_MOVES[square].block_bits.push_back(square - 6);
        }
        if (square / 8 > 1 && square % 8 < 7) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 15); // SES.
            KNIGHT_MOVES[square].block_bits.push_back(square - 15);
        }
        if (square / 8 > 1 && square % 8 > 0) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 17); // SWS.
            KNIGHT_MOVES[square].block_bits.push_back(square - 17);
        }
        if (square / 8 > 0 && square % 8 > 1) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square - 10); // ESE.
            KNIGHT_MOVES[square].block_bits.push_back(square - 10);
        }
        if (square / 8 < 7 && square % 8 > 1) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 6); // WNW.
            KNIGHT_MOVES[square].block_bits.push_back(square + 6);
        }
        if (square / 8 < 6 && square % 8 > 0) {
            KNIGHT_MOVES[square].reach |= 1ULL << (square + 15); // NWN.
            KNIGHT_MOVES[square].block_bits.push_back(square + 15);
        }

        // Sort the block bits.
        std::sort(KNIGHT_MOVES[square].block_bits.begin(), 
                KNIGHT_MOVES[square].block_bits.end());

        // Set the moves.
        std::vector<int>* blockers = &(KNIGHT_MOVES[square].block_bits);
        KNIGHT_MOVES[square].move_set.resize(std::pow(2, blockers->size()));

        // Iterate over end occupancies.
        for (int i = 0; i < std::pow(2, blockers->size()); i++) {
            uint64_t pos = KNIGHT_MOVES[square].reach;
        
            // Create the position with masked ends.
            int index = 0;
            for (auto j = blockers->begin(); j != blockers->end(); j++) {
                pos = (pos & ~(1ULL << *j)) | ((1ULL & i >> index) << *j);
                index++;
            }

            std::vector<uint16_t>* moves_set = &(KNIGHT_MOVES[square].
                move_set[moveSetIndex(pos, &KNIGHT_MOVES[square])]);
            
            // Set the moves.
            for (uint64_t shift = 0; shift < 64; shift++) {
                if (((pos >> shift) & 1ULL) == 1) {
                    moves_set->push_back(square | shift << 6 | NORMAL | pKNIGHT);
                }
            }
        }
    }
}

/**
 * Gets a bitboard of knight checkers.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param square: The square of the king in check.
 * @param checkers_only: A pointer to a bit board of checkers only.
 * @return: Bitboard of knight checkers.
 */
uint64_t Game::getKnightCheckers(Computed* moves, Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    *checkers_only |= moves->KNIGHT_MOVES[square].reach & this->sides[1 - this->turn] & this->knights;
    return result;
}
