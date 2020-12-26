
#include <cmath>
#include <algorithm>

#include "pawn_moves.hpp"

/**
 * Computes pawn moves.
 * @param PAWN_MOVES: The array of pawn moves structs.
 * @param EN_PASSANT_MOVES: Array of en-passant moves structs. 32 is the offset
 *  to index a moves struct. move_set index of 1 is for left ep, 0 is for right
 *  ep.
 * @param DOUBLE_PUSH: An array of moves structs for double pawn push blocks.
 */
void Game::computePawnMoves(Moves PAWN_MOVES[][48], Moves EN_PASSANT_MOVES[16],
        Moves DOUBLE_PUSH[16]) {
    for (int player = BLACK; player <= WHITE; player++) {
        int left = player == BLACK ? -9 : 7;
        int right = player == BLACK ? -7 : 9;
        int forward_push = player == BLACK ?  -8 : 8;
        int double_push = player == BLACK ? - 16 : 16;
        
        for (int square = 8; square <= 55; square++) {
            uint64_t pos = 0; // The position for move generation.
            PAWN_MOVES[player][square - 8].reach = 0;

            // Create the attack reach.
            if (square % 8 != 0) {
                PAWN_MOVES[player][square - 8].reach |= 1ULL << (square + 
                        left);
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                        left);
                pos |= 1ULL << (square + left);
            }
            if (square % 8 != 7) {
                PAWN_MOVES[player][square - 8].reach |= 1ULL << (square + 
                        right);
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                        right);
                pos |= 1ULL << (square + right);
            }

            // Add one push block bit.
            PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                    forward_push);
            pos |= 1ULL << (square + forward_push);

            // Add two push block bit.
            if ((player == WHITE && square / 8 == 1) || (player == BLACK && 
                    square / 8 == 6)) {
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + 
                        double_push);
                pos |= 1ULL << (square + double_push);
            }
            
            // Sort the block bits.
            std::sort(PAWN_MOVES[player][square - 8].block_bits.begin(), 
                    PAWN_MOVES[player][square - 8].block_bits.end());

            // Set the moves.
            std::vector<int>* blockers = &(PAWN_MOVES[player][square - 8].
                    block_bits);
            PAWN_MOVES[player][square - 8].move_set.resize(std::pow(2, 
                    blockers->size()));
            
            // En-passant captures
            if (player == WHITE && square / 8 == 4) { // En-passant.
                EN_PASSANT_MOVES[square - 24].move_set.resize(2);
                std::vector<uint16_t>* r_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[0]);
                std::vector<uint16_t>* l_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[1]);
                r_ep_set->push_back(square | (square + right) << 6 | 
                        EN_PASSANT | pKNIGHT);
                l_ep_set->push_back(square | (square + left) << 6 | EN_PASSANT |
                        pKNIGHT);
            } else if (player == BLACK && square / 8 == 3) { // En-passant.
                EN_PASSANT_MOVES[square - 24].move_set.resize(2);
                std::vector<uint16_t>* r_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[0]);
                std::vector<uint16_t>* l_ep_set = &(EN_PASSANT_MOVES[square - 
                        24].move_set[1]);
                r_ep_set->push_back(square | (square + right) << 6 | 
                        EN_PASSANT | pKNIGHT);
                l_ep_set->push_back(square | (square + left) << 6 | EN_PASSANT |
                        pKNIGHT);
            }

            // Double push blocks
            if (player == WHITE && square / 8 == 1) {
                DOUBLE_PUSH[square - 8].move_set.resize(1);
                std::vector<uint16_t>* move = &(DOUBLE_PUSH[square - 8].
                        move_set[0]);
                move->push_back(square | (square + 16) << 6 | NORMAL | pKNIGHT);
            } else if (player == BLACK && square / 8 == 6) {
                DOUBLE_PUSH[square - 40].move_set.resize(1);
                std::vector<uint16_t>* move = &(DOUBLE_PUSH[square - 40].
                        move_set[0]);
                move->push_back(square | (square - 16) << 6 | NORMAL | pKNIGHT);
            }

            // Iterate over occupancies.
            for (int i = 0; i < std::pow(2, blockers->size()); i++) {            
                // Create the position with masked ends.
                int index = 0;
                for (auto j = blockers->begin(); j != blockers->end(); j++) {
                    pos = (pos & ~(1ULL << *j)) | ((1ULL & (i >> index)) << *j);
                    index++;
                }

                std::vector<uint16_t>* moves_set = &(PAWN_MOVES[player][square -
                        8].move_set[moveSetIndex(pos, &PAWN_MOVES[player][
                        square - 8])]);

                if (player == WHITE) {
                    if (square % 8 != 7 && square / 8 == 6) { // Right promo.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pROOK);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 7) { // Normal right capture.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | NORMAL | pKNIGHT);
                        }
                    }

                    if (square % 8 != 0 && square / 8 == 6) { // Left promo
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pROOK);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 0) { // Normal left capture.
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    NORMAL | pKNIGHT);
                        }
                    }

                    // Move up promo.
                    if (square / 8 == 6 && !bitAt(pos, square + forward_push)) {
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pKNIGHT);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pBISHOP);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pROOK);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pQUEEN);
                    } else if (!bitAt(pos, square + forward_push)) {
                        // Normal move up.
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | NORMAL | pKNIGHT);
                    }

                    if (square / 8 == 1) { // Starting square.
                        if (!bitAt(pos, square + forward_push) && !bitAt(pos,
                                square + double_push)) { // Move up.
                            moves_set->push_back(square | (square + 
                                    double_push) << 6 | NORMAL | pKNIGHT);
                        }
                    }
                } else {
                    if (square % 8 != 7 && square / 8 == 1) { // Right promo.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pROOK);
                            moves_set->push_back(square | (square + right) << 
                                    6 | PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 7) { // Normal right capture.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 
                                    6 | NORMAL | pKNIGHT);
                        }
                    }

                    if (square % 8 != 0 && square / 8 == 1) { // Left promo
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pROOK);
                            moves_set->push_back(square | (square + left) << 6 |
                                    PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 0) { // Normal left capture.
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 |
                                    NORMAL | pKNIGHT);
                        }
                    }

                    if (square / 8 == 1 && !bitAt(pos, square + forward_push)) {
                        // Move up promo.
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pKNIGHT);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pBISHOP);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pROOK);
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | PROMOTION | pQUEEN);
                    } else if (!bitAt(pos, square + forward_push)) {
                        // Normal move up.
                        moves_set->push_back(square | (square + forward_push) <<
                                6 | NORMAL | pKNIGHT);
                    }

                    if (square / 8 == 6) { // Starting square.
                        if (!bitAt(pos, square + forward_push) && !bitAt(pos, 
                                square + double_push)) { // Move up.
                            moves_set->push_back(square | (square + 
                                    double_push) << 6 | NORMAL | pKNIGHT);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Get the arguments for the pawn index function for pawns.
 * @param game: Pointer to game struct.
 * @param square: Square of pawn.
 */
uint64_t Game::pawnMoveArgs(Square square) {
    bool turn = this->turn;
    return (this->sides[!turn] | (this->sides[turn] & files[square % 8]));
}

/**
 * Returns a bitboard of all pawn checkers.
 * @param game: The game struct pointer of the current position.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Game::getPawnCheckers(Square square, uint64_t* checkers_only) {
    uint64_t enemy_pawns = this->sides[1 - this->turn] & this->pawns;
    uint64_t result = 0;
    int rank_offset = this->turn ? 8 : -8;
    if (square % 8 != 0) {
        int pawn_sq = square + rank_offset - 1;
        if ((1ULL << pawn_sq) & enemy_pawns) {
            *checkers_only |= 1ULL << pawn_sq;
        }
    }

    if (square % 8 != 7) {
        int pawn_sq = square + rank_offset + 1;
        if ((1ULL << pawn_sq) & enemy_pawns) {
            *checkers_only |= 1ULL << pawn_sq;
        }
    }

    return result;
}
