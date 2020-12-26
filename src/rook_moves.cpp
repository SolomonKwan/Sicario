
#include <cmath>
#include <algorithm>

#include "rook_moves.hpp"

/**
 * Sets the moves of the rook for a particular square and occupancy (according
 * to MSBs) and creates the moves. This only happens if the family of moves has
 * not been set yet.
 * @param square: The square that the rook is on.
 * @param move_family: The struct holding the moves family for rooks.
 */
void Game::setRookMoves(int square, MovesStruct* move_family) {
    move_family->reach = 0;

    // Set the reach.
    for (int bit : move_family->block_bits) {
        if (bit < square) { // To left or below.
            if (bit / 8 == square / 8) { // To left.
                int index = square - 1;
                while (index >= bit) {
                    move_family->reach |= 1ULL << index;
                    index--;
                }
            } else { // Below.
                int index = square - 8;
                while (index >= bit) {
                    move_family->reach |= 1ULL << index;
                    index -= 8;
                }
            }
        } else { // To right or above.
            if (bit / 8 == square / 8) { // To right.
                int index = square + 1;
                while (index <= bit) {
                    move_family->reach |= 1ULL << index;
                    index++;
                }
            } else { // Above.
                int index = square + 8;
                while (index <= bit) {
                    move_family->reach |= 1ULL << index;
                    index += 8;
                }
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

        // Set upward moves.
        index = square + 8;
        while (((pos >> index) & 1ULL) != 0) {
            if (index > 63) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index += 8;
        }

        // Set right moves.
        index = square + 1;
        while (((pos >> index) & 1ULL) != 0) {
            if (index == (square / 8) * 8 + 8) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index++;
        }

        // Set downward moves.
        index = square - 8;
        while (((pos >> index) & 1ULL) != 0) {
            if (index < 0) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index -= 8;
        }

        // Set left moves.
        index = square - 1;
        while (((pos >> index) & 1ULL) != 0) {
            if (index == (square / 8) * 8 - 1) break;
            moves_set->push_back(square | index << 6 | NORMAL | pKNIGHT);
            index--;
        }
    }
}

/**
 * Computes the rook moves for the corner squares and calls the function
 * to set the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset into the rook attack sets.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRCornerMoves(int square, int* offset, std::vector<int>* ROOK_INDEX, 
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    ROOK_INDEX[square].resize(4096);

    for (int vOcc = 0; vOcc < 64; vOcc++) {
        for (int hOcc = 0; hOcc < 64; hOcc++) {
            // Build position.
            int index;
            uint64_t pos = 0;
            
            // Retrieve and set horizontal bits.
            square % 8 == 0 ? index = 5 : index = 0 ;
            for (int k = hStart; k <= hEnd; k++) {
                pos |= ((hOcc >> index) & 1ULL) << k;
                square % 8 == 0 ? index-- : index++;
            }

            // Retrieve and set vertical bits.
            square / 8 == 0 ? index = 5 : index = 0;
            for (int x = vStart; x <= vEnd; x += 8) {
                pos |= ((vOcc >> index) & 1ULL) << x;
                square / 8 == 0 ? index-- : index++;
            }

            // Index creation.
            int rookIndex = (
                ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                        rookShifts[square]
            );

            int move_index = MSB(hOcc) + 7 * MSB(vOcc) + *offset;
            ROOK_INDEX[square][rookIndex] = move_index;

            if (ROOK_MOVES[move_index].reach == UNSET) {
                ROOK_MOVES[move_index].block_bits.push_back(
                    square / 8 == 0 ? square + 8 * (7 - MSB(vOcc)) :
                            square - 8 * (7 - MSB(vOcc))
                );
                ROOK_MOVES[move_index].block_bits.push_back(
                    square % 8 == 0 ? square + (7 - MSB(hOcc)) :
                            square - (7 - MSB(hOcc))
                );
                std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                        ROOK_MOVES[move_index].block_bits.end());
                Game::setRookMoves(square, &(ROOK_MOVES[move_index]));
            }
        }
    }

    *offset += 49;
}

/**
 * Computes the rook moves for the left and right squares and calls the 
 * function to set the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRLRSideMoves(int square, int* offset, std::vector<int>* ROOK_INDEX,
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

    ROOK_INDEX[square].resize(2048);

    int v1_occs = std::pow(2, (v1_End - v1_Start) / 8 + 1);
    int v2_occs = std::pow(2, (v2_End - v2_Start) / 8 + 1);

    int offset_add;

    for (int hOcc = 0; hOcc < 64; hOcc++) {
        for (int vOcc1 = 0; vOcc1 < v1_occs; vOcc1++) {
            for (int vOcc2 = 0; vOcc2 < v2_occs; vOcc2++) {
                // Build position.
                int index;
                uint64_t pos = 0;
                
                // Retrieve and set horizontal bits.
                square % 8 == 0 ? index = 5 : index = 0;
                for (int k = hStart; k <= hEnd; k++) {
                    pos |= ((hOcc >> index) & 1ULL) << k;
                    square % 8 == 0 ? index-- : index++;
                }

                // Retrieve and set vertical1 bits.
                if (v1_occs != 1) {
                    index = 0;
                    for (int x = v1_Start; x <= v1_End; x += 8) {
                        pos |= ((vOcc1 >> index) & 1ULL) << x;
                        index++;
                    }
                }

                // Retrieve and set vertical2 bits.
                if (v2_occs != 1) {
                    index = (v2_End - v2_Start) / 8;
                    for (int x = v2_Start; x <= v2_End; x += 8) {
                        pos |= ((vOcc2 >> index) & 1ULL) << x;
                        index--;
                    }
                }

                // Index creation.
                uint64_t rookIndex = (
                    ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                            rookShifts[square]
                );

                int move_index;
                if (square / 8 == 1) { // A2 H2
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                } else if (square / 8 == 2) { // A3 H3
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 14 * MSB(vOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else if (square / 8 == 3) { // A4 H4
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 21 * MSB(vOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square / 8 == 4) { // A5 H5
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 21 * MSB(vOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square / 8 == 5) { // A6 H6
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 14 * MSB(vOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else { // A7 H7
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                }
                
                if (ROOK_MOVES[move_index].reach == UNSET) {
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square + 8 * ((7 - square / 8) - MSB(vOcc2))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square - 8 * ((square / 8) - MSB(vOcc1))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square % 8 == 0 ? square + (7 - MSB(hOcc)) :
                                square - (7 - MSB(hOcc))
                    );
                    std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                            ROOK_MOVES[move_index].block_bits.end());
                    setRookMoves(square, &(ROOK_MOVES[move_index]));
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Computes the rook moves for the upper and lower squares and calls the
 * function to set the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRULSideMoves(int square, int* offset, std::vector<int>* ROOK_INDEX,
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    ROOK_INDEX[square].resize(2048);

    int h1_occs = std::pow(2, h1_End - h1_Start + 1);
    int h2_occs = std::pow(2, h2_End - h2_Start + 1);

    int offset_add;

    for (int vOcc = 0; vOcc < 64; vOcc++) {
        for (int hOcc1 = 0; hOcc1 < h1_occs; hOcc1++) {
            for (int hOcc2 = 0; hOcc2 < h2_occs; hOcc2++) {
                // Build position.
                int index;
                uint64_t pos = 0;
                
                // Retrieve and set vertical bits.
                square / 8 == 0 ? index = 5 : index = 0;
                for (int k = vStart; k <= vEnd; k += 8) {
                    pos |= ((vOcc >> index) & 1ULL) << k;
                    square / 8 == 0 ? index-- : index++;
                }

                // Retrieve and set horizontal1 bits.
                if (h1_occs != 1) {
                    index = 0;
                    for (int x = h1_Start; x <= h1_End; x++) {
                        pos |= ((hOcc1 >> index) & 1ULL) << x;
                        index++;
                    }
                }

                // Retrieve and set horizontal2 bits.
                if (h2_occs != 1) {
                    index = h2_End - h2_Start;
                    for (int x = h2_Start; x <= h2_End; x++) {
                        pos |= ((hOcc2 >> index) & 1ULL) << x;
                        index--;
                    }
                }

                // Index creation.
                uint64_t rookIndex = (
                    ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                            rookShifts[square]
                );

                int move_index;
                if (square % 8 == 1) { // B1 B8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                } else if (square % 8 == 2) { // C1 C8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 14 * MSB(hOcc2) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else if (square % 8 == 3) { // D1 D8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 21 * MSB(hOcc2) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square % 8 == 4) { // E1 E8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 21 * MSB(hOcc1) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 84;
                } else if (square % 8 == 5) { // F1 F8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 14 * MSB(hOcc1) + 
                            *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 70;
                } else { // G1 G8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + *offset;
                    ROOK_INDEX[square][rookIndex] = move_index;
                    offset_add = 42;
                }

                if (ROOK_MOVES[move_index].reach == UNSET) {
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square / 8 == 0 ? square + 8 * (7 - MSB(vOcc)) :
                            square - 8 * (7 - MSB(vOcc))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square - ((square % 8) - MSB(hOcc1))
                    );
                    ROOK_MOVES[move_index].block_bits.push_back(
                        square + (7 - square % 8) - MSB(hOcc2)
                    );
                    std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                            ROOK_MOVES[move_index].block_bits.end());
                    setRookMoves(square, &(ROOK_MOVES[move_index]));
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Computes the rook moves for the center squares and calls the function to set
 * the moves and reach.
 * @param square: The corner square of interest.
 * @param offset: The index offset.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Array of move structs for each rook move family.
 */
void Game::computeRCentreMoves(int square, int* offset, std::vector<int>* ROOK_INDEX,
        MovesStruct* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

    ROOK_INDEX[square].resize(1024);

    int h1_occs = std::pow(2, h1_End - h1_Start + 1);
    int h2_occs = std::pow(2, h2_End - h2_Start + 1);
    int v1_occs = std::pow(2, (v1_End - v1_Start) / 8 + 1);
    int v2_occs = std::pow(2, (v2_End - v2_Start) / 8 + 1);

    int offset_add;
    for (int hOcc1 = 0; hOcc1 < h1_occs; hOcc1++) {
        for (int hOcc2 = 0; hOcc2 < h2_occs; hOcc2++) {
            for (int vOcc1 = 0; vOcc1 < v1_occs; vOcc1++) {
                for (int vOcc2 = 0; vOcc2 < v2_occs; vOcc2++) {
                    // Build position.
                    int index;
                    uint64_t pos = 0;

                    // Retrieve and set horizontal1 bits.
                    if (h1_occs != 1) {
                        index = 0;
                        for (int x = h1_Start; x <= h1_End; x++) {
                            pos |= ((hOcc1 >> index) & 1ULL) << x;
                            index++;
                        }
                    }

                    // Retrieve and set horizontal2 bits.
                    if (h2_occs != 1) {
                        index = h2_End - h2_Start;
                        for (int x = h2_Start; x <= h2_End; x++) {
                            pos |= ((hOcc2 >> index) & 1ULL) << x;
                            index--;
                        }
                    }

                    // Retrieve and set vertical1 bits.
                    if (v1_occs != 1) {
                        index = 0;
                        for (int x = v1_Start; x <= v1_End; x += 8) {
                            pos |= ((vOcc1 >> index) & 1ULL) << x;
                            index++;
                        }
                    }

                    // Retrieve and set vertical2 bits.
                    if (v2_occs != 1) {
                        index = (v2_End - v2_Start) / 8;
                        for (int x = v2_Start; x <= v2_End; x += 8) {
                            pos |= ((vOcc2 >> index) & 1ULL) << x;
                            index--;
                        }
                    }

                    // Index creation.
                    uint64_t rookIndex = (
                        ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
                                rookShifts[square]
                    );

                    int move_index;
                    if (square == B2 || square == B7 || square == G2 || 
                            square == G7) {
                        move_index = MSB((h1_occs != 1 ? hOcc1 : hOcc2)) + 
                                6 * MSB((v1_occs != 1 ? vOcc1 : vOcc2)) + 
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 36;
                    } else if (square == C2 || square == F2 || square == C7 || 
                            square == F7 || square == B3 || square == G3 || 
                            square == B6 || square == G6) {
                        move_index = MSB(h1_occs == 32 ? hOcc1 : 
                                    (h2_occs == 32 ? hOcc2 : 
                                    (v1_occs == 32 ? vOcc1 : vOcc2))) +
                                6 * MSB(h1_occs == 2 ? hOcc1 : 
                                    (h2_occs == 2 ? hOcc2 : 
                                    (v1_occs == 2 ? vOcc1 : vOcc2))) +
                                12 * MSB(h1_occs == 16 ? hOcc1 : 
                                    (h2_occs == 16 ? hOcc2 : 
                                    (v1_occs == 16 ? vOcc1 : vOcc2))) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 60;
                    } else if (square == D2 || square == E2 || square == B4 || 
                            square == G4 || square == B5 || square == G5 || 
                            square == D7 || square == E7) {
                        move_index = MSB(h1_occs == 32 ? hOcc1 : 
                                    (h2_occs == 32 ? hOcc2 : 
                                    (v1_occs == 32 ? vOcc1 : vOcc2))) +
                                6 * MSB(h1_occs == 8 ? hOcc1 : 
                                    (h2_occs == 8 ? hOcc2 : 
                                    (v1_occs == 8 ? vOcc1 : vOcc2))) +
                                24 * MSB(h1_occs == 4 ? hOcc1 : 
                                    (h2_occs == 4 ? hOcc2 : 
                                    (v1_occs == 4 ? vOcc1 : vOcc2))) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 72;
                    } else if (square == C3 || square == F3 || square == C6 || 
                            square == F6) {
                        move_index = MSB(h1_occs == 16 ? hOcc1 : hOcc2) +
                                25 * MSB(h1_occs == 2 ? hOcc1 : hOcc2) +
                                5 * MSB(v1_occs == 16 ? vOcc1 : vOcc2) +
                                50 * MSB(v1_occs == 2 ? vOcc1 : vOcc2) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 100;
                    } else if (square == D3 || square == E3 || square == C4 || 
                            square == F4 || square == C5 || square == F5 || 
                            square == D6 || square == E6) {
                        move_index = MSB(h1_occs == 16 ? hOcc1 :
                                    (h2_occs == 16 ? hOcc2 :
                                    (v1_occs == 16 ? vOcc1 :vOcc2))) +
                                5 * MSB(h1_occs == 8 ? hOcc1 :
                                    (h2_occs == 8 ? hOcc2 :
                                    (v1_occs == 8 ? vOcc1 : vOcc2))) +
                                20 * MSB(h1_occs == 4 ? hOcc1 :
                                    (h2_occs == 4 ? hOcc2 :
                                    (v1_occs == 4 ? vOcc1 : vOcc2))) +
                                60 * MSB(h1_occs == 2 ? hOcc1 :
                                    (h2_occs == 2 ? hOcc2 :
                                    (v1_occs == 2 ? vOcc1 : vOcc2))) +
                                    *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 120;
                    } else {
                        move_index = MSB(h1_occs == 8 ? hOcc1 : hOcc2) +
                                4 * MSB(v1_occs == 8 ? vOcc1 : vOcc2) +
                                16 * MSB(v1_occs == 4 ? vOcc1 : vOcc2) +
                                48 * MSB(h1_occs == 4 ? hOcc1 : hOcc2) +
                                *offset;
                        ROOK_INDEX[square][rookIndex] = move_index;
                        offset_add = 144;
                    }

                    if (ROOK_MOVES[move_index].reach == UNSET) {
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square - 8 * ((square / 8) - MSB(vOcc1))
                        );
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square - ((square % 8) - MSB(hOcc1))
                        );
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square + (7 - square % 8) - MSB(hOcc2)
                        );
                        ROOK_MOVES[move_index].block_bits.push_back(
                            square + 8 * ((7 - square / 8) - MSB(vOcc2))
                        );

                        std::sort(ROOK_MOVES[move_index].block_bits.begin(),
                                ROOK_MOVES[move_index].block_bits.end());
                        setRookMoves(square, &(ROOK_MOVES[move_index]));
                    }
                }
            }
        }
    }

    *offset += offset_add;
}

/**
 * Returns the index from ROOK_INDEX into ROOK_MOVES based on the square and 
 * occupancy.
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 * @return: Index into ROOK_MOVES.
 */
const int Game::rookIndex(const uint64_t pos, Square square) {
    return ((pos & rookMasks[square]) * rookMagicNums[square]) >> 
            rookShifts[square];
}

/**
 * Computes the index (in this case the key) into the ROOK_BLOCK moves map.
 * @param pos: The uint64_t with possible destination squares from the param
 *      square set.
 * @param moves: The precomputed moves.
 * @param square: The square from which the piece moves.
 * @param game: A Game struct pointer.
 * @return: Index into the ROOK_BLOCK array.
 */
const int Game::rookBlockIndex(uint64_t pos, Computed* moves, Square square) {
    return pos & this->getRookFamily(moves, square)->reach;
}

/**
 * Gets a bitboard of the rays between the rook (queen) checkers and the king
 * being checked.
 * @param game: A pointer to game struct.
 * @param moves: A pointer to precomputed moves.
 * @param square: The square of the king in check.
 * @param checkers: Bitboard of checkers.
 * @return: Check rays of rooks and queens.
 */
uint64_t Game::getRookCheckRays(Computed* moves, Square square, uint64_t* checkers) {
    uint64_t result = 0;
    Square king_sq = this->piece_list[this->turn][0];
    uint64_t king_rays = this->getRookFamily(moves, king_sq)->reach & ~this->sides[this->turn];
    *checkers |= (this->queens | this->rooks) & this->sides[1 - this->turn] & king_rays;
    PieceType queen = this->turn ? B_QUEEN : W_QUEEN;
    PieceType rook = this->turn ? B_ROOK : W_ROOK;

    for (int i = 0; i < this->piece_index[queen]; i++) {
        Square queen_sq = this->piece_list[queen][i];
        if (queen_sq % 8 == king_sq % 8 || queen_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily(moves, (Square) queen_sq)->reach & 
                    ~this->sides[1 - this->turn] & king_rays);
        }
    }

    for (int i = 0; i < this->piece_index[rook]; i++) {
        Square rook_sq = this->piece_list[rook][i];
        if (rook_sq % 8 == king_sq % 8 || rook_sq / 8 == king_sq / 8) {
            result |= (this->getRookFamily(moves, (Square) rook_sq)->reach & 
                    ~this->sides[1 - this->turn] & king_rays);
        }
    }

    return result;
}

/**
 * Iterates through the squares and call functions to compute rook moves and
 * calculate indices.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Move struct to store the moves.
 */
void Game::computeRookMoves(std::vector<int>* ROOK_INDEX, MovesStruct* ROOK_MOVES) {
    // Set initial reach to UNSET.
    for (int i = 0; i < 4900; i++) {
        ROOK_MOVES[i].reach = UNSET;
    }

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            Game::computeRCornerMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        } else if (square % 8 == 0 || square % 8 == 7) {
            Game::computeRLRSideMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        } else if (square / 8 == 0 || square / 8 == 7) {
            Game::computeRULSideMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        } else {
            Game::computeRCentreMoves(square, &offset, ROOK_INDEX, ROOK_MOVES);
        }
    }
}

/**
 * Computes the blocking moves (and captures in case of check) for rooks on each
 * square.
 * @param ROOK_BLOCKS: A vector of move structs.
 */
void Game::computeRookBlocks(MovesStruct* ROOK_BLOCKS) {
    std::tuple<int, int> pairs[4] = {
        std::make_tuple(8, 1), std::make_tuple(1, -8), std::make_tuple(-8, -1),
        std::make_tuple(-1, 8)
    };
    int directions[4] = {8, 1, -8, -1};

    for (int square = A1; square <= H8; square++) {
        
        // Single ray traversals.
        for (int direction : directions) {
            if ((direction == 8 && square / 8 == 7) || (direction == 1 && 
                    square % 8 == 7) || (direction == -8 && square / 8 == 0) || 
                    (direction == -1 && square % 8 == 0)) {
                continue;
            }

            int start = square + direction;
            int end_pt = std::abs(direction) == 8 ? (start % 8 + 
                    (direction == 8 ? 64 : -8)) : (8 * (start / 8) + 
                    (direction == 1 ? 8 : -1));

            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &ROOK_BLOCKS[square].checked_moves;
            while (start != end_pt) {
                // moves[pos] = std::vector<uint16_t>(); WHY DOESN'T WORK????
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

            if ((square / 8 == 0 && (ray1 == -8 || ray2 == -8)) || 
                    (square / 8 == 7 && (ray1 == 8 || ray2 == 8)) || 
                    (square % 8 == 0 && (ray1 == -1 || ray2 == -1)) || 
                    (square % 8 == 7 && (ray1 == 1 || ray2 == 1)) ) {
                continue;
            }

            int start1 = square + ray1, start2 = square + ray2; 
            int end1, end2;

            end1 = std::abs(ray1) == 8 ? (start1 % 8 + (ray1 == 8 ? 64 : -8)) :
                    (8 * (start1 / 8) + (ray1 == 1 ? 8 : -1));
            end2 = std::abs(ray2) == 8 ? (start2 % 8 + (ray2 == 8 ? 64 : -8)) :
                    (8 * (start2 / 8) + (ray2 == 1 ? 8 : -1));
            
            std::unordered_map<uint64_t, std::vector<uint16_t>>* moves = 
                    &ROOK_BLOCKS[square].checked_moves;

            while (start1 != end1) {
                start2 = square + ray2;
                while (start2 != end2) {
                    uint16_t move1 = square | start1 << 6 | NORMAL | pKNIGHT;
                    uint16_t move2 = square | start2 << 6 | NORMAL | pKNIGHT;
                    std::vector<uint16_t> moveset = {move1, move2};
                    moves->insert(std::make_pair(1ULL << start1 | 1ULL << start2, 
                            moveset));
                    start2 += ray2;
                }
                start1 += ray1;
            };
        };
    }
}

/**
 * Finds and returns a pointer to a rook move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* Game::getRookFamily(Computed* moves, Square square) {
    &moves->ROOK_MOVES[moves->ROOK_INDEX[square][rookIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
    return &moves->ROOK_MOVES[moves->ROOK_INDEX[square][
            rookIndex(this->sides[BLACK] | this->sides[WHITE], square)]];
}

/**
 * Finds and returns a pointer to a rook block move family.
 * @param game: A pointer to a game struct representing the state of the game.
 * @param moves: A struct of the precomputed moves.
 * @param square: The square on which the rook is on.
 * @return Pointer to moves struct.
 */
MovesStruct* getRookBlockFamily(Game* game, Computed* moves, Square square) {
    return nullptr;
}
