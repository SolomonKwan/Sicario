
#include <algorithm>
#include <cmath>
#include <set>

#include "bishop_moves.hpp"
#include "movegen.hpp"

/**
 * Sets the reach of the bishop for a particular square and occupancy (according
 * to MSBs) and creates the moves. This only happens if the family of moves has
 * not been set yet.
 * 
 * @param square: The square that the bishop is on.
 * @param move_family: Pointer to struct holding the moves family for bishop.
 */
void Game::setBishopMoves(int square, Moves* move_family) {
    move_family->reach = 0;

    // Set the reach.
    for (int bit : move_family->block_bits) {
        if (bit > square && bit % 8 > square % 8) { // Upper right.
            int index = square + 9;
            while (index <= bit) {
                move_family->reach |= 1ULL << index;
                index += 9;
            }
        } else if (bit > square && bit % 8 < square % 8) { // Upper left.
            int index = square + 7;
            while (index <= bit) {
                move_family->reach |= 1ULL << index;
                index += 7;
            }
        } else if (bit < square && bit % 8 > square % 8) { // Lower right.
            int index = square - 7;
            while (index >= bit) {
                move_family->reach |= 1ULL << index;
                index -= 7;
            }
        } else { // Lower left.
            int index = square - 9;
            while (index >= bit) {
                move_family->reach |= 1ULL << index;
                index -= 9;
            }
        }
    }

    // Set the moves.
    std::vector<int>* blockers = &move_family->block_bits;
    move_family->move_set.resize(std::pow(2, blockers->size()));

    // Iterate over end occupancies.
    for (int i = 0; i < std::pow(2, blockers->size()); i++) {
        uint64_t pos = move_family->reach;
        
        // Create the position with masked ends.
        int index = 0;
        for (auto j = blockers->begin(); j != blockers->end(); j++) {
            pos = (pos & ~(1ULL << *j)) | ((1ULL & i >> index) << *j);
            index++;
        }

        std::vector<uint16_t>* moves_set = &(move_family->
                move_set[moveSetIndex(pos, move_family)]);

        // Set upper right moves.
        index = square + 9;
        while (((pos >> index) & 1ULL) != 0) {
            if (index > 63 || index % 8 <= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index += 9;
        }

        // Set upper left moves.
        index = square + 7;
        while (((pos >> index) & 1ULL) != 0) {
            if (index > 63 || index % 8 >= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index += 7;
        }

        // Set lower right moves.
        index = square - 7;
        while (((pos >> index) & 1ULL) != 0) {
            if (index < 0 || index % 8 <= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index -= 7;
        }

        // Set lower left moves.
        index = square - 9;
        while (((pos >> index) & 1ULL) != 0) {
            if (index < 0 || index % 8 >= square % 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index -= 9;
        }
    }
}

/**
 * Computes the bishop move indices for the corner squares and calls the 
 * function to set the moves and reach.
 * 
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBCornerMoves(int square, int* offset, 
        std::vector<int>* BISHOP_INDEX, Moves* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start = square + (square / 8 <= 3 ? 8 : -8) + 
            (square % 8 <= 3 ? 1 : -1);
    int end = square + (square / 8 <= 3 ? 48 : -48) + 
            (square % 8 <= 3 ? 6 : -6);

    BISHOP_INDEX[square].resize(64);
    std::set<int> indices;
    for (int occ = 0; occ < 64; occ++) {
        // Build position.
        int index, change;
        uint64_t pos = 0;
            
        // Retrieve and set bits.
        if (square == A1) change = 9;
        if (square == A8) change = -7;
        if (square == H1) change = 7;
        if (square == H8) change = -9;
        index = 5;
        for (int k = start; k != end; k += change) {
            pos |= ((occ >> index) & 1ULL) << k;
            index--;
        }
        pos |= ((occ >> index) & 1ULL) << end;

        // Index creation.
        int bishopIndex = (
            ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
                    bishopShifts[square]
        );

        int move_index = MSB(occ) + *offset;
        BISHOP_INDEX[square][bishopIndex] = move_index;

        indices.insert(move_index);
        if (BISHOP_MOVES[move_index].reach == UNSET) {
            BISHOP_MOVES[move_index].block_bits.push_back(
                square + change * (7 - MSB(occ))
            );
            std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                    BISHOP_MOVES[move_index].block_bits.end());
            Game::setBishopMoves(square, &(BISHOP_MOVES[move_index]));
        }
    }

    *offset += 7;
}

/**
 * Computes the bishop move indices for the left and right squares and calls the 
 * function to set the moves and reach.
 * 
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBLRSideMoves(int square, int* offset, 
        std::vector<int>* BISHOP_INDEX, Moves* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start1 = square % 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square % 8 == 0 ? 9 : -9) * (square % 8 == 0 ? 
            6 - square / 8 : square / 8 - 1);
    int start2 = square % 8 == 0 ? square - 7 : square + 7;
    int end2 = square + (square % 8 == 0 ? -7 : 7) * (square % 8 == 0 ? 
            square / 8 - 1 : 6 - square / 8);

    BISHOP_INDEX[square].resize(32);

    int bits1 = square % 8 == 0 ? (end1 % 8 - start1 % 8 + 1) : 
            (start1 % 8 - end1 % 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square % 8 == 0 ? (end2 % 8 - start2 % 8 + 1) : 
            (start2 % 8 - end2 % 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

    std::set<int> indices;
    int offset_add;
    for (int occ1 = 0; occ1 < size1; occ1++) {
        for (int occ2 = 0; occ2 < size2; occ2++) {
            // Build position.
            int index, change;
            uint64_t pos = 0;
            
            // Retrieve and set right diagonal bits.
            change = square % 8 == 0 ? 9 : -9;
            index = bits1 - 1;
            if (size1 != 1) {
                for (int i = start1; i != end1; i += change) {
                    pos |= ((occ1 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ1 >> index) & 1ULL) << end1;
            }

            // Retrieve and set left diagonal bits.
            change = square % 8 == 0 ? -7 : 7;
            index = bits2 - 1;
            if (size2 != 1) {
                for (int i = start2; i != end2; i += change) {
                    pos |= ((occ2 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ2 >> index) & 1ULL) << end2;
            }

            // Index creation.
            int bishopIndex = (
                ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
                        bishopShifts[square]
            );

            int move_index;
            if (square == A2 || square == A7 || square == H2 || square == H7) {
                move_index = MSB(size1 == 32 ? occ1 : occ2) + *offset;
                offset_add = 6;
            } else if (square == A3 || square == A6 || square == H3 || 
                    square == H6) {
                move_index = MSB(size1 == 16 ? occ1 : occ2) + 
                        5 * MSB(size1 == 2 ? occ1 : occ2) + *offset;
                offset_add = 10;
            } else {
                move_index = MSB(size1 == 8 ? occ1 : occ2) + 
                        4 * MSB(size1 == 4 ? occ1 : occ2) + *offset;
                offset_add = 12;
            }

            BISHOP_INDEX[square][bishopIndex] = move_index;
            indices.insert(move_index);
            if (BISHOP_MOVES[move_index].reach == UNSET) {
                BISHOP_MOVES[move_index].block_bits.push_back( // Right diag.
                    square + (square % 8 == 0 ? 9 : -9) * 
                            (square % 8 == 0 ? 7 - square / 8 - MSB(occ1) : 
                            square / 8 - MSB(occ1))
                );
                BISHOP_MOVES[move_index].block_bits.push_back( // Left diag.
                    square + (square % 8 == 0 ? -7 : 7) * 
                            (square % 8 == 0 ? square / 8 - MSB(occ2) : 
                            7 - square / 8 - MSB(occ2))
                );

                std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                        BISHOP_MOVES[move_index].block_bits.end());
                Game::setBishopMoves(square, &(BISHOP_MOVES[move_index]));
            }
        }
    }

    *offset += offset_add;
}

/**
 * Computes the bishop move indices for the upper and lower squares and calls 
 * the function to set the moves and reach.
 * 
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBULSideMoves(int square, int* offset, 
        std::vector<int>* BISHOP_INDEX, Moves* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start1 = square / 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square / 8 == 0 ? 9 : -9) * (square / 8 == 0 ? 
            6 - square % 8 : square % 8 - 1); // Right diagonal.
    int start2 = square / 8 == 0 ? square + 7 : square - 7;
    int end2 = square + (square / 8 == 0 ? 7 : -7) * (square / 8 == 0 ? 
            square % 8 - 1 : 6 - square % 8); // Left diagonal.

    BISHOP_INDEX[square].resize(32);
    int bits1 = square / 8 == 0 ? (end1 / 8 - start1 / 8 + 1) : 
            (start1 / 8 - end1 / 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square / 8 == 0 ? (end2 / 8 - start2 / 8 + 1) : 
            (start2 / 8 - end2 / 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

    std::set<int> indices;
    int offset_add;
    for (int occ1 = 0; occ1 < size1; occ1++) {
        for (int occ2 = 0; occ2 < size2; occ2++) {
            // Build position.
            int index, change;
            uint64_t pos = 0;
            
            // Retrieve and set right diagonal bits.
            change = square / 8 == 0 ? 9 : -9;
            index = bits1 - 1;
            if (size1 != 1) {
                for (int i = start1; i != end1; i += change) {
                    pos |= ((occ1 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ1 >> index) & 1ULL) << end1;
            }

            // Retrieve and set left diagonal bits.
            change = square / 8 == 0 ? 7 : -7;
            index = bits2 - 1;
            if (size2 != 1) {
                for (int i = start2; i != end2; i += change) {
                    pos |= ((occ2 >> index) & 1ULL) << i;
                    index--;
                }
                pos |= ((occ2 >> index) & 1ULL) << end2;
            }

            // Index creation.
            int bishopIndex = (
                ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
                        bishopShifts[square]
            );

            int move_index;
            if (square == B1 || square == B8 || square == G1 || square == G8) {
                move_index = MSB(size1 == 32 ? occ1 : occ2) + *offset;
                offset_add = 6;
            } else if (square == C1 || square == C8 || square == F1 || 
                    square == F8) {
                move_index = MSB(size1 == 16 ? occ1 : occ2) + 
                        5 * MSB(size1 == 2 ? occ1 : occ2) + *offset;
                offset_add = 10;
            } else {
                move_index = MSB(size1 == 8 ? occ1 : occ2) + 
                        4 * MSB(size1 == 4 ? occ1 : occ2) + *offset;
                offset_add = 12;
            }
            BISHOP_INDEX[square][bishopIndex] = move_index;

            indices.insert(move_index);
            if (BISHOP_MOVES[move_index].reach == UNSET) {
                BISHOP_MOVES[move_index].block_bits.push_back( // Right diag.
                    square + (square / 8 == 0 ? 9 : -9) * 
                            (square / 8 == 0 ? 7 - square % 8 - MSB(occ1) : 
                            square % 8 - MSB(occ1))
                );
                BISHOP_MOVES[move_index].block_bits.push_back( // Left diag.
                    square + (square / 8 == 0 ? 7 : -7) * 
                            (square / 8 == 0 ? square % 8 - MSB(occ2) : 
                            7 - square % 8 - MSB(occ2))
                );

                std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                        BISHOP_MOVES[move_index].block_bits.end());
                Game::setBishopMoves(square, &(BISHOP_MOVES[move_index]));
            }
        }
    }
    *offset += offset_add;
}

/**
 * Computes the bishop move indices for the centre squares and calls the 
 * function to set the moves and reach.
 * 
 * @param square: The centre square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the 
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of move structs for each bishop move family.
 */
void Game::computeBCentreMoves(int sq, int* offset, std::vector<int>* BISHOP_INDEX, 
        Moves* BISHOP_MOVES) {
    int start1 = sq + 9; // Upper right.
    int start2 = sq - 7; // Lower right.
    int start3 = sq - 9; // Lower left.
    int start4 = sq + 7; // Upper left.

    int end1, end2, end3, end4;

    int ind = start1;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind += 9;
    }
    ind -= 9;
    end1 = ind;

    ind = start2;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind -= 7;
    }
    ind += 7;
    end2 = ind;

    ind = start3;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind -= 9;
    }
    ind += 9;
    end3 = ind;

    ind = start4;
    while (ind / 8 != 0 && ind / 8 != 7 && ind % 8 != 0 && ind % 8 != 7) {
        ind += 7;
    }
    ind -= 7;
    end4 = ind;


    if (sq / 8 == 1 || sq / 8 == 6 || sq % 8 == 1 || sq % 8 == 6) {
        BISHOP_INDEX[sq].resize(32);
    } else if (sq / 8 == 2 || sq / 8 == 5 || sq % 8 == 2 || sq % 8 == 5) {
        BISHOP_INDEX[sq].resize(128);
    } else {
        BISHOP_INDEX[sq].resize(512);
    }

    std::set<int> indices;
    
    int bits1 = end1 % 8 - start1 % 8 + 1;
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = end2 % 8 - start2 % 8 + 1;
    int size2 = std::pow(2, bits2); // Left diagonal.
    int bits3 = start3 % 8 - end3 % 8 + 1;
    int size3 = std::pow(2, bits3); // Right diagonal.
    int bits4 = start4 % 8 - end4 % 8 + 1;
    int size4 = std::pow(2, bits4); // Left diagonal.

    int offset_add;

    for (int occ1 = 0; occ1 < size1; occ1++) {
        for (int occ2 = 0; occ2 < size2; occ2++) {
            for (int occ3 = 0; occ3 < size3; occ3++) {
                for (int occ4 = 0; occ4 < size4; occ4++) {
                     // Build position.
                    int ind;
                    uint64_t pos = 0;
                    int msb1 = -1, msb2 = -1, msb3 = -1, msb4 = -1;

                    // Retrieve and set upper right bits.
                    ind = bits1 - 1;
                    if (size1 != 1) {
                        for (int i = start1; i <= end1; i += 9) {
                            pos |= ((occ1 >> ind) & 1ULL) << i;
                            if (((occ1 >> ind) & 1ULL) == 1ULL && msb1 == -1) {
                                msb1 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb1 == -1) msb1 = end1 + 9;

                    // Retrieve and set lower right bits.
                    ind = bits2 - 1;
                    if (size2 != 1) {
                        for (int i = start2; i >= end2; i -= 7) {
                            pos |= ((occ2 >> ind) & 1ULL) << i;
                            if (((occ2 >> ind) & 1ULL) == 1ULL && msb2 == -1) {
                                msb2 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb2 == -1) msb2 = end2 - 7;

                    // Retrieve and set lower left bits.
                    ind = bits3 - 1;
                    if (size3 != 1) {
                        for (int i = start3; i >= end3; i -= 9) {
                            pos |= ((occ3 >> ind) & 1ULL) << i;
                            if (((occ3 >> ind) & 1ULL) == 1ULL && msb3 == -1) {
                                msb3 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb3 == -1) msb3 = end3 - 9;

                    // Retrieve and set upper left bits.
                    ind = bits4 - 1;
                    if (size4 != 1) {
                        for (int i = start4; i <= end4; i += 7) {
                            pos |= ((occ4 >> ind) & 1ULL) << i;
                            if (((occ4 >> ind) & 1ULL) == 1ULL && msb4 == -1) {
                                msb4 = i;
                            }
                            ind--;
                        }
                    }
                    if (msb4 == -1) msb4 = end4 + 7;

                    // Index creation.
                    int bishopIndex = (
                        ((pos & bishopMasks[sq]) * bishopMagicNums[sq]) >> 
                                bishopShifts[sq]
                    );

                    int move_index;
                    if (sq == B2 || sq == B7 || sq == G2 || 
                            sq == G7) {
                        move_index = MSB(
                            size1 == 32 ? occ1 : (
                            size2 == 32 ? occ2 : (
                            size3 == 32 ? occ3 : occ4))
                        ) + *offset;
                        offset_add = 6;
                    } else if (sq == C2 || sq == F2 || sq == C7 || 
                            sq == F7 || sq == B3 || sq == G3 || 
                            sq == B6 || sq == G6) {
                        move_index = MSB(
                            size1 == 16 ? occ1 : (
                            size2 == 16 ? occ2 : (
                            size3 ==  16 ? occ3 : occ4))) + 
                        5 * MSB(size1 == 2 ? occ1 : (
                            size2 == 2 ? occ2 : (
                            size3 ==  2 ? occ3 : occ4))) + *offset;
                        offset_add = 10;
                    } else if (sq == D2 || sq == E2 || sq == B4 || 
                            sq == G4 || sq == B5 || sq == G5 || 
                            sq == D7 || sq == E7) {
                        move_index = MSB(
                            size1 == 8 ? occ1 : (
                            size2 == 8 ? occ2 : (
                            size3 ==  8 ? occ3 : occ4))) + 
                        4 * MSB(size1 == 4 ? occ1 : (
                            size2 == 4 ? occ2 : (
                            size3 ==  4 ? occ3 : occ4))) + *offset;
                        offset_add = 12;
                    } else if (sq == C3 || sq == F3 || sq == C6 || 
                            sq == F6) {
                        if (sq == C3) {
                            move_index = MSB(occ1) + 5 * MSB(occ2) + 10 * 
                                    MSB(occ3) + 20 * MSB(occ4) + *offset;
                        } else if (sq == F3) {
                            move_index = MSB(occ4) + 5 * MSB(occ2) + 10 * 
                                    MSB(occ3) + 20 * MSB(occ1) + *offset;
                        } else if (sq == C6) {
                            move_index = MSB(occ2) + 5 * MSB(occ1) + 10 * 
                                    MSB(occ3) + 20 * MSB(occ4) + *offset;
                        } else {
                            move_index = MSB(occ3) + 5 * MSB(occ2) + 10 * 
                                    MSB(occ1) + 20 * MSB(occ4) + *offset;
                        }
                        offset_add = 40;
                    } else if (sq == D3 || sq == E3 || sq == C4 || 
                            sq == F4 || sq == C5 || sq == F5 || 
                            sq == D6 || sq == E6) {
                        if (sq % 8 == 2) {
                            move_index = MSB(size1 == 8 ? occ1 : occ2) + 
                                4 * MSB(size1 == 4 ? occ1 : occ2) +
                                12 * MSB(occ3) +
                                24 * MSB(occ4) + *offset; 
                        } else if (sq % 8 == 5) {
                            move_index = MSB(size3 == 8 ? occ3 : occ4) + 
                                4 * MSB(size3 == 4 ? occ3 : occ4) +
                                12 * MSB(occ1) +
                                24 * MSB(occ2) + *offset;
                        } else if (sq / 8 == 2) {
                            move_index = MSB(size1 == 8 ? occ1 : occ4) + 
                                4 * MSB(size1 == 4 ? occ1 : occ4) +
                                12 * MSB(occ2) +
                                24 * MSB(occ3) + *offset;
                        } else {
                            move_index = MSB(size2 == 8 ? occ2 : occ3) + 
                                4 * MSB(size2 == 4 ? occ2 : occ3) +
                                12 * MSB(occ1) +
                                24 * MSB(occ4) + *offset;
                        }
                        offset_add = 48;
                    } else {
                        if (sq == D4) {
                            move_index = MSB(occ1) + 4 * MSB(occ2) + 12 * 
                                    MSB(occ3) + 36 * MSB(occ4) + *offset;
                        } else if (sq == D5) {
                            move_index = MSB(occ2) + 4 * MSB(occ1) + 12 * 
                                    MSB(occ3) + 36 * MSB(occ4) + *offset;
                        } else if (sq == E4) {
                            move_index = MSB(occ4) + 4 * MSB(occ2) + 12 * 
                                    MSB(occ3) + 36 * MSB(occ1) + *offset;
                        } else {
                            move_index = MSB(occ3) + 4 * MSB(occ2) + 12 * 
                                    MSB(occ1) + 36 * MSB(occ4) + *offset;
                        }
                        offset_add = 108;
                    }
                    indices.insert(move_index);
                    BISHOP_INDEX[sq][bishopIndex] = move_index;

                    if (BISHOP_MOVES[move_index].reach == UNSET) {
                        BISHOP_MOVES[move_index].block_bits.push_back(msb1);                        
                        BISHOP_MOVES[move_index].block_bits.push_back(msb2);
                        BISHOP_MOVES[move_index].block_bits.push_back(msb3);
                        BISHOP_MOVES[move_index].block_bits.push_back(msb4);

                        std::sort(BISHOP_MOVES[move_index].block_bits.begin(),
                                BISHOP_MOVES[move_index].block_bits.end());
                        Game::setBishopMoves(sq, &(BISHOP_MOVES[move_index]));
                    }
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Returns the index from BISHOP_INDEX into BISHOP_MOVES based on the square and 
 * occupancy.
 * 
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 */
const int Game::bishopIndex(const uint64_t pos, Square square) {
    return ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> 
            bishopShifts[square];
}

/**
 * Gets a bitboard of the rays between the bishop (queen) checkers and the king
 * being checked.
 * 
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers_only: Pointer to bit board of checkers only.
 */
uint64_t Game::getBishopCheckRays(Computed* moves, Square square, uint64_t* checkers_only) {
    uint64_t result = 0;
    bool turn = this->turn;
    Square king_sq = this->piece_list[turn][0];
    uint64_t king_rays = this->getBishopFamily(moves, king_sq)->reach & 
            ~this->sides[turn];
    *checkers_only |= (this->queens | this->bishops) & this->sides[!turn] & 
            king_rays;
    PieceType queen = turn ? B_QUEEN : W_QUEEN;
    PieceType bishop = turn ? B_BISHOP : W_BISHOP;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square piece = this->piece_list[queen][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 
                8)) {
            result |= (this->getBishopFamily(moves, (Square) piece)->reach & 
                    ~this->sides[!turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[bishop]; i++) {
        Square piece = this->piece_list[bishop][i];
        if (std::abs(piece % 8 - king_sq % 8) == std::abs(piece / 8 - king_sq / 
                8)) {
            result |= (this->getBishopFamily(moves, (Square) piece)->reach & 
                    ~this->sides[!turn] & king_rays);
        }
    }

    return result;
}

/**
 * Iterates through the squares and call functions to compute bishop moves and
 * calculate indices.
 * 
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of bishop move sets to store precomputed moves.
 */
void Game::computeBishopMoves(std::vector<int>* BISHOP_INDEX, Moves* BISHOP_MOVES) {
    // Set initial reach to UNSET.
    for (int i = 0; i < 1428; i++) {
        BISHOP_MOVES[i].reach = UNSET;
    }

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            Game::computeBCornerMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        } else if (square % 8 == 0 || square % 8 == 7) {
            Game::computeBLRSideMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        } else if (square / 8 == 0 || square / 8 == 7) {
            Game::computeBULSideMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        } else {
            Game::computeBCentreMoves(square, &offset, BISHOP_INDEX, BISHOP_MOVES);
        }
    }
}

/**
 * Computes the blocking moves (and captures in case of check) for bishops on 
 * each square.
 * 
 * @param BISHOP_BLOCKS: A vector of move structs.
 */
void Game::computeBishopBlocks(Moves* BISHOP_BLOCKS) {
    std::tuple<int, int> pairs[4] = {
        std::make_tuple(9, -7), std::make_tuple(-7, -9), std::make_tuple(-9, 7),
        std::make_tuple(7, 9)
    };
    int directions[4] = {9, -7, -9, 7};

    for (int square = A1; square <= H8; square++) {
        
        // Single ray traversals.
        for (int direction : directions) {
            if ((direction == 9 && (square % 8 == 7 || square / 8 == 7)) || 
                    (direction == -7 && (square % 8 == 7 || square / 8 == 0)) ||
                    (direction == -9 && (square % 8 == 0 || square / 8 == 0)) ||
                    (direction == 7 && (square % 8 == 0 || square / 8 == 7))) {
                continue;
            }

            int start = square + direction;
            int end_pt = start;
            while (end_pt / 8 != 0 && end_pt / 8 != 7 && end_pt % 8 != 0 && 
                    end_pt % 8 != 7) {
                end_pt += direction;
            }
            end_pt += direction;

            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &BISHOP_BLOCKS[square].checked_moves;
            while (start != end_pt) {
                uint16_t move = square | start << 6 | NORMAL | pKNIGHT;
                std::vector<uint16_t> moveset = {move};
                moves->insert(std::make_pair(1ULL << start, moveset));
                start += direction;
            }
        }

        // Pair ray traversal.
        for (std::tuple<int, int> tuple : pairs) {
            int ray1 = std::get<0>(tuple);
            int ray2 = std::get<1>(tuple);

            if ((square / 8 == 0 && (ray1 < 0 || ray2 < 0)) || 
                    (square / 8 == 7 && (ray1 > 0 || ray2 > 0)) || 
                    (square % 8 == 0 && (ray1 == -9 || ray1 == 7 || 
                    ray2 == -9 || ray2 == 7)) || (square % 8 == 7 && 
                    (ray1 == 9 || ray1 == -7 || ray2 == 9 || ray2 == -7))) {
                continue;
            }

            int start1 = square + ray1, start2 = square + ray2; 
            int end1 = start1, end2 = start2;
            
            while (end1 / 8 != 0 && end1 / 8 != 7 && end1 % 8 != 0 && 
                    end1 % 8 != 7) {
                end1 += ray1;
            }
            end1 += ray1;

            while (end2 / 8 != 0 && end2 / 8 != 7 && end2 % 8 != 0 && 
                    end2 % 8 != 7) {
                end2 += ray2;
            }
            end2 += ray2;
            
            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &BISHOP_BLOCKS[square].checked_moves;

            while (start1 != end1) {
                start2 = square + ray2;
                while (start2 != end2) {
                    uint16_t move1 = square | start1 << 6 | NORMAL | pKNIGHT;
                    uint16_t move2 = square | start2 << 6 | NORMAL | pKNIGHT;
                    std::vector<uint16_t> moveset = {move1, move2};
                    moves->insert(std::make_pair(1ULL << start1 | 1ULL << 
                            start2, moveset));
                    start2 += ray2;
                }
                start1 += ray1;
            };
        };
    }
}

/**
 * Finds and returns a pointer to a bishop move family.
 * 
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the bishop is on.
 */
Moves* Game::getBishopFamily(Computed* moves, Square square) {
    return &moves->BISHOP_MOVES[moves->BISHOP_INDEX[square][
            this->bishopIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}
