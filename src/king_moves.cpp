
#include <algorithm>
#include <cmath>

#include "movegen.hpp"

/**
 * Compute the king moves.
 * @param KING_MOVES: The array of king moves to be computed.
 */
void Game::computeKingMoves(Moves* KING_MOVES) {
    // Compute the moves and reach.
    for (int square = A1; square <= H8; square++) {
        KING_MOVES[square].reach = 0;
        
        if (square / 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square + 8);
            KING_MOVES[square].block_bits.push_back(square + 8);
        }
        if (square / 8 != 7 && square % 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square + 9);
            KING_MOVES[square].block_bits.push_back(square + 9);
        }
        if (square % 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square + 1);
            KING_MOVES[square].block_bits.push_back(square + 1);
        }
        if (square / 8 != 0 && square % 8 != 7) {
            KING_MOVES[square].reach |= 1ULL << (square - 7);
            KING_MOVES[square].block_bits.push_back(square - 7);
        }
        if (square / 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square - 8);
            KING_MOVES[square].block_bits.push_back(square - 8);
        }
        if (square / 8 != 0 && square % 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square - 9);
            KING_MOVES[square].block_bits.push_back(square - 9);
        }
        if (square % 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square - 1);
            KING_MOVES[square].block_bits.push_back(square - 1);
        }
        if (square / 8 != 7 && square % 8 != 0) {
            KING_MOVES[square].reach |= 1ULL << (square + 7);
            KING_MOVES[square].block_bits.push_back(square + 7);
        }

        // Sort the block bits.
        std::sort(KING_MOVES[square].block_bits.begin(), 
                KING_MOVES[square].block_bits.end());

        // Set the moves.
        std::vector<int>* blockers = &(KING_MOVES[square].block_bits);
        KING_MOVES[square].move_set.resize(std::pow(2, blockers->size()));

        // Iterate over end occupancies.
        for (int i = 0; i < std::pow(2, blockers->size()); i++) {
            uint64_t pos = KING_MOVES[square].reach;
        
            // Create the position with masked ends.
            int index = 0;
            for (auto j = blockers->begin(); j != blockers->end(); j++) {
                pos = (pos & ~(1ULL << *j)) | ((1ULL & (i >> index)) << *j);
                index++;
            }

            std::vector<uint16_t>* moves_set = &(KING_MOVES[square].
                move_set[moveSetIndex(pos, &KING_MOVES[square])]);
            
            // Set the moves.
            for (uint64_t shift = 0; shift < 64; shift++) {
                if (((pos >> shift) & 1ULL) == 1) {
                    moves_set->push_back(square | shift << 6 | NORMAL | pKNIGHT);
                }
            }
        }
    }
}