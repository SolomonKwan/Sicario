
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <set>

#include "constants.hpp"
#include "movegen.hpp"
#include "game.hpp"

/**
 * @param pos: A bitboard.
 */
void displayBB(uint64_t pos) {
    // Convert number to binary string.
    std::string positionString = std::bitset<64>(pos).to_string();
    
    // Reverse each line then print.
    std::cout << "\n";
    for (int i = 0; i < 8; i++) {
        std::string line = positionString.substr(8 * i, 8);
        std::reverse(line.begin(), line.end());
        std::cout << line << '\n';
    }

    std::cout << '\n';
}

/**
 * Finds and returns the position of the most significant set bit. If not bits
 * are set, returns 0.
 * 
 * @param n: The integer to find the MSB.
 * @return: The most significant bit position.
 */
int MSB(int n) {
    if (n == 0) return 0;

    int msb = 0;
    while (n != 0) {
        n /= 2;
        msb++;
    }

    return msb;
}

/**
 * Returns true if the nths bit of pos is set, else false.
 * 
 * @param pos: A bitboard.
 * @param n: The wanted bit.
 * @return: True if bit is set, else false.
 */
bool bitAt(uint64_t pos, int n) {
    return ((pos >> n) & 1ULL) == 1ULL;
}

/**
 * Calculates the index into the move_set of a particular move family.
 * 
 * @param masked_reach: A bitboard of the reach of the piece with own pieces
 *      masked out.
 * @param move_family: A pointer to Moves struct holding the move family. The
 *      block_bits vector MUST hold the squares in order from smallest to 
 *      largest.
 * @return: Index into the move_set.
 */
int moveSetIndex(uint64_t masked_reach, MovesStruct* move_family) {
    int index = 0, result = 0;
    for (auto i = move_family->block_bits.begin(); i != move_family->block_bits.end(); i++) {
        result |= ((masked_reach >> *i) & 1ULL) << index;
        index++;
    }
    return result;
}

/**
 * Compute the castling moves.
 * @param CASTLING_MOVES: Array of move struct pointers.
 */
std::vector<MovesStruct> computeCastling() {
    std::vector<MovesStruct> CASTLING_MOVES(4);
    CASTLING_MOVES[WQSC].move_set.resize(1);
    std::vector<Move>* moves_set = &(CASTLING_MOVES[WQSC].move_set[0]);
    moves_set->push_back(E1 | C1 << 6 | CASTLING | pKNIGHT);

    CASTLING_MOVES[WKSC].move_set.resize(1);
    moves_set = &CASTLING_MOVES[WKSC].move_set[0];
    moves_set->push_back(E1 | G1 << 6 | CASTLING | pKNIGHT);

    CASTLING_MOVES[BQSC].move_set.resize(1);
    moves_set = &CASTLING_MOVES[BQSC].move_set[0];
    moves_set->push_back(E8 | C8 << 6 | CASTLING | pKNIGHT);

    CASTLING_MOVES[BKSC].move_set.resize(1);
    moves_set = &CASTLING_MOVES[BKSC].move_set[0];
    moves_set->push_back(E8 | G8 << 6 | CASTLING | pKNIGHT);

    return CASTLING_MOVES;
}

/**
 * Sets the reach of the bishop for a particular square and occupancy (according
 * to MSBs) and creates the moves. This only happens if the family of moves has
 * not been set yet.
 * 
 * @param square: The square that the bishop is on.
 * @param move_family: Pointer to struct holding the moves family for bishop.
 */
void setBishopMoves(int square, MovesStruct* move_family) {
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

        std::vector<Move>* moves_set = &(move_family->
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
void computeBCornerMoves(int square, int* offset, std::vector<MovesStruct>* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start = square + (square / 8 <= 3 ? 8 : -8) + 
            (square % 8 <= 3 ? 1 : -1);
    int end = square + (square / 8 <= 3 ? 48 : -48) + 
            (square % 8 <= 3 ? 6 : -6);

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
        int move_index = MSB(occ) + *offset;

        if ((*BISHOP_MOVES)[move_index].reach == UNSET) {
            (*BISHOP_MOVES)[move_index].block_bits.push_back(square + change * (7 - MSB(occ)));
            std::sort((*BISHOP_MOVES)[move_index].block_bits.begin(),
                    (*BISHOP_MOVES)[move_index].block_bits.end());
            setBishopMoves(square, &((*BISHOP_MOVES)[move_index]));
        }
    }

    *offset += 7;
}

std::vector<int> computeBCornerIndices(int square, int* offset) {
    std::vector<int> bishop;
    // The diagonal start and end squares.
    int start = square + (square / 8 <= 3 ? 8 : -8) + 
            (square % 8 <= 3 ? 1 : -1);
    int end = square + (square / 8 <= 3 ? 48 : -48) + 
            (square % 8 <= 3 ? 6 : -6);

    bishop.resize(64);
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
        bishop[bishopIndex] = move_index;
    }

    *offset += 7;
    return bishop;
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
void computeBLRSideMoves(int square, int* offset, std::vector<MovesStruct>* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start1 = square % 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square % 8 == 0 ? 9 : -9) * (square % 8 == 0 ? 6 - square / 8 : square / 8 - 1);
    int start2 = square % 8 == 0 ? square - 7 : square + 7;
    int end2 = square + (square % 8 == 0 ? -7 : 7) * (square % 8 == 0 ? square / 8 - 1 : 6 - square / 8);

    int bits1 = square % 8 == 0 ? (end1 % 8 - start1 % 8 + 1) : (start1 % 8 - end1 % 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square % 8 == 0 ? (end2 % 8 - start2 % 8 + 1) : (start2 % 8 - end2 % 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

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

            if ((*BISHOP_MOVES)[move_index].reach == UNSET) {
                (*BISHOP_MOVES)[move_index].block_bits.push_back( // Right diag.
                    square + (square % 8 == 0 ? 9 : -9) * 
                            (square % 8 == 0 ? 7 - square / 8 - MSB(occ1) : 
                            square / 8 - MSB(occ1))
                );
                (*BISHOP_MOVES)[move_index].block_bits.push_back( // Left diag.
                    square + (square % 8 == 0 ? -7 : 7) * 
                            (square % 8 == 0 ? square / 8 - MSB(occ2) : 
                            7 - square / 8 - MSB(occ2))
                );

                std::sort((*BISHOP_MOVES)[move_index].block_bits.begin(),
                        (*BISHOP_MOVES)[move_index].block_bits.end());
                setBishopMoves(square, &((*BISHOP_MOVES)[move_index]));
            }
        }
    }

    *offset += offset_add;
}

std::vector<int> computeBLRSideIndices(int square, int* offset) {
    std::vector<int> indices;
    // The diagonal start and end squares.
    int start1 = square % 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square % 8 == 0 ? 9 : -9) * (square % 8 == 0 ? 
            6 - square / 8 : square / 8 - 1);
    int start2 = square % 8 == 0 ? square - 7 : square + 7;
    int end2 = square + (square % 8 == 0 ? -7 : 7) * (square % 8 == 0 ? 
            square / 8 - 1 : 6 - square / 8);

    indices.resize(32);

    int bits1 = square % 8 == 0 ? (end1 % 8 - start1 % 8 + 1) : 
            (start1 % 8 - end1 % 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square % 8 == 0 ? (end2 % 8 - start2 % 8 + 1) : 
            (start2 % 8 - end2 % 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

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
            int bishopIndex = ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> bishopShifts[square];

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

            indices[bishopIndex] = move_index;
        }
    }

    *offset += offset_add;
    return indices;
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
void computeBULSideMoves(int square, int* offset, std::vector<MovesStruct>* BISHOP_MOVES) {
    // The diagonal start and end squares.
    int start1 = square / 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square / 8 == 0 ? 9 : -9) * (square / 8 == 0 ? 6 - square % 8 : square % 8 - 1); // Right diagonal.
    int start2 = square / 8 == 0 ? square + 7 : square - 7;
    int end2 = square + (square / 8 == 0 ? 7 : -7) * (square / 8 == 0 ? square % 8 - 1 : 6 - square % 8); // Left diagonal.

    int bits1 = square / 8 == 0 ? (end1 / 8 - start1 / 8 + 1) : 
            (start1 / 8 - end1 / 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square / 8 == 0 ? (end2 / 8 - start2 / 8 + 1) : 
            (start2 / 8 - end2 / 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

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

            int move_index;
            if (square == B1 || square == B8 || square == G1 || square == G8) {
                move_index = MSB(size1 == 32 ? occ1 : occ2) + *offset;
                offset_add = 6;
            } else if (square == C1 || square == C8 || square == F1 || 
                    square == F8) {
                move_index = MSB(size1 == 16 ? occ1 : occ2) + 5 * MSB(size1 == 2 ? occ1 : occ2) + *offset;
                offset_add = 10;
            } else {
                move_index = MSB(size1 == 8 ? occ1 : occ2) + 4 * MSB(size1 == 4 ? occ1 : occ2) + *offset;
                offset_add = 12;
            }

            if ((*BISHOP_MOVES)[move_index].reach == UNSET) {
                (*BISHOP_MOVES)[move_index].block_bits.push_back( // Right diag.
                    square + (square / 8 == 0 ? 9 : -9) * 
                            (square / 8 == 0 ? 7 - square % 8 - MSB(occ1) : 
                            square % 8 - MSB(occ1))
                );
                (*BISHOP_MOVES)[move_index].block_bits.push_back( // Left diag.
                    square + (square / 8 == 0 ? 7 : -7) * 
                            (square / 8 == 0 ? square % 8 - MSB(occ2) : 
                            7 - square % 8 - MSB(occ2))
                );

                std::sort((*BISHOP_MOVES)[move_index].block_bits.begin(),
                        (*BISHOP_MOVES)[move_index].block_bits.end());
                setBishopMoves(square, &((*BISHOP_MOVES)[move_index]));
            }
        }
    }
    *offset += offset_add;
}

std::vector<int> computeBULSideIndices(int square, int* offset) {
    std::vector<int> indices;

    // The diagonal start and end squares.
    int start1 = square / 8 == 0 ? square + 9 : square - 9;
    int end1 = square + (square / 8 == 0 ? 9 : -9) * (square / 8 == 0 ? 
            6 - square % 8 : square % 8 - 1); // Right diagonal.
    int start2 = square / 8 == 0 ? square + 7 : square - 7;
    int end2 = square + (square / 8 == 0 ? 7 : -7) * (square / 8 == 0 ? 
            square % 8 - 1 : 6 - square % 8); // Left diagonal.

    indices.resize(32);
    int bits1 = square / 8 == 0 ? (end1 / 8 - start1 / 8 + 1) : 
            (start1 / 8 - end1 / 8 + 1);
    int size1 = std::pow(2, bits1); // Right diagonal.
    int bits2 = square / 8 == 0 ? (end2 / 8 - start2 / 8 + 1) : 
            (start2 / 8 - end2 / 8 + 1);
    int size2 = std::pow(2, bits2); // Left diagonal.

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
            indices[bishopIndex] = move_index;
        }
    }
    *offset += offset_add;

    return indices;
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
void computeBCentreMoves(int sq, int* offset, std::vector<MovesStruct>* BISHOP_MOVES) {
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

                    if ((*BISHOP_MOVES)[move_index].reach == UNSET) {
                        (*BISHOP_MOVES)[move_index].block_bits.push_back(msb1);
                        (*BISHOP_MOVES)[move_index].block_bits.push_back(msb2);
                        (*BISHOP_MOVES)[move_index].block_bits.push_back(msb3);
                        (*BISHOP_MOVES)[move_index].block_bits.push_back(msb4);

                        std::sort((*BISHOP_MOVES)[move_index].block_bits.begin(),
                                (*BISHOP_MOVES)[move_index].block_bits.end());
                        setBishopMoves(sq, &((*BISHOP_MOVES)[move_index]));
                    }
                }
            }
        }
    }

    *offset += offset_add;
}

std::vector<int> computeBCentreIndices(int sq, int* offset) {
    std::vector<int> indices;

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
        indices.resize(32);
    } else if (sq / 8 == 2 || sq / 8 == 5 || sq % 8 == 2 || sq % 8 == 5) {
        indices.resize(128);
    } else {
        indices.resize(512);
    }
    
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
                    indices[bishopIndex] = move_index;
                }
            }
        }
    }

    *offset += offset_add;
    return indices;
}

/**
 * Returns the index from BISHOP_INDEX into BISHOP_MOVES based on the square and 
 * occupancy.
 * 
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 */
const int bishopIndex(const uint64_t pos, Square square) {
    return ((pos & bishopMasks[square]) * bishopMagicNums[square]) >> bishopShifts[square];
}

/**
 * Iterates through the squares and call functions to compute bishop moves and
 * calculate indices.
 * 
 * @param BISHOP_INDEX: Array of vectors holding ints where indices into the
 *      bishop attack sets are set.
 * @param BISHOP_MOVES: Array of bishop move sets to store precomputed moves.
 */
std::vector<MovesStruct> computeBishopMoves() {
    std::vector<MovesStruct> moves(1428);

    // Set initial reach to UNSET.
    for (int i = 0; i < 1428; i++) {
        moves[i].reach = UNSET;
    }

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            computeBCornerMoves(square, &offset, &moves);
        } else if (square % 8 == 0 || square % 8 == 7) {
            computeBLRSideMoves(square, &offset, &moves);
        } else if (square / 8 == 0 || square / 8 == 7) {
            computeBULSideMoves(square, &offset, &moves);
        } else {
            computeBCentreMoves(square, &offset, &moves);
        }
    }

    return moves;
}

std::vector<std::vector<int>> computeBishopIndices() {
    std::vector<std::vector<int>> indices(64);

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            indices[square] = computeBCornerIndices(square, &offset);
        } else if (square % 8 == 0 || square % 8 == 7) {
            indices[square] = computeBLRSideIndices(square, &offset);
        } else if (square / 8 == 0 || square / 8 == 7) {
            indices[square] = computeBULSideIndices(square, &offset);
        } else {
            indices[square] = computeBCentreIndices(square, &offset);
        }
    }

    return indices;
}

/**
 * Computes the blocking moves (and captures in case of check) for bishops on 
 * each square.
 * 
 * @param BISHOP_BLOCKS: A vector of move structs.
 */
std::vector<MovesStruct> computeBishopBlocks() {
    std::vector<MovesStruct> blocks(64);
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

            std::unordered_map<uint64_t, std::vector<Move>>* moves = 
                    &blocks[square].checked_moves;
            while (start != end_pt) {
                Move move = square | start << 6 | NORMAL | pKNIGHT;
                std::vector<Move> moveset = {move};
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
            
            std::unordered_map<uint64_t, std::vector<Move>>* moves = 
                    &blocks[square].checked_moves;

            while (start1 != end1) {
                start2 = square + ray2;
                while (start2 != end2) {
                    Move move1 = square | start1 << 6 | NORMAL | pKNIGHT;
                    Move move2 = square | start2 << 6 | NORMAL | pKNIGHT;
                    std::vector<Move> moveset = {move1, move2};
                    moves->insert(std::make_pair(1ULL << start1 | 1ULL << 
                            start2, moveset));
                    start2 += ray2;
                }
                start1 += ray1;
            };
        };
    }

    return blocks;
}

/**
 * Compute the king moves.
 * @param KING_MOVES: The array of king moves to be computed.
 */
std::vector<MovesStruct> computeKingMoves() {
    std::vector<MovesStruct> moves(64);

    // Compute the moves and reach.
    for (int square = A1; square <= H8; square++) {
        moves[square].reach = 0;
        
        if (square / 8 != 7) {
            moves[square].reach |= 1ULL << (square + 8);
            moves[square].block_bits.push_back(square + 8);
        }
        if (square / 8 != 7 && square % 8 != 7) {
            moves[square].reach |= 1ULL << (square + 9);
            moves[square].block_bits.push_back(square + 9);
        }
        if (square % 8 != 7) {
            moves[square].reach |= 1ULL << (square + 1);
            moves[square].block_bits.push_back(square + 1);
        }
        if (square / 8 != 0 && square % 8 != 7) {
            moves[square].reach |= 1ULL << (square - 7);
            moves[square].block_bits.push_back(square - 7);
        }
        if (square / 8 != 0) {
            moves[square].reach |= 1ULL << (square - 8);
            moves[square].block_bits.push_back(square - 8);
        }
        if (square / 8 != 0 && square % 8 != 0) {
            moves[square].reach |= 1ULL << (square - 9);
            moves[square].block_bits.push_back(square - 9);
        }
        if (square % 8 != 0) {
            moves[square].reach |= 1ULL << (square - 1);
            moves[square].block_bits.push_back(square - 1);
        }
        if (square / 8 != 7 && square % 8 != 0) {
            moves[square].reach |= 1ULL << (square + 7);
            moves[square].block_bits.push_back(square + 7);
        }

        // Sort the block bits.
        std::sort(moves[square].block_bits.begin(), moves[square].block_bits.end());

        // Set the moves.
        std::vector<int>* blockers = &(moves[square].block_bits);
        moves[square].move_set.resize(std::pow(2, blockers->size()));

        // Iterate over end occupancies.
        for (int i = 0; i < std::pow(2, blockers->size()); i++) {
            uint64_t pos = moves[square].reach;
        
            // Create the position with masked ends.
            int index = 0;
            for (auto j = blockers->begin(); j != blockers->end(); j++) {
                pos = (pos & ~(1ULL << *j)) | ((1ULL & (i >> index)) << *j);
                index++;
            }

            std::vector<Move>* moves_set = &(moves[square].move_set[moveSetIndex(pos, &moves[square])]);
            
            // Set the moves.
            for (uint64_t shift = 0; shift < 64; shift++) {
                if (((pos >> shift) & 1ULL) == 1) {
                    moves_set->push_back(square | shift << 6 | NORMAL | pKNIGHT);
                }
            }
        }
    }
    return moves;
}

/**
 * Compute the knight moves.
 * @param KNIGHT_MOVES: Array of knight moves to be computed.
 */
std::vector<MovesStruct> computeKnightMoves() {
    std::vector<MovesStruct> knight(64);
    // Compute the moves and reach.
    for (int square = A1; square <= H8; square++) {
        // Compute the reach.
        knight[square].reach = 0;
        
        if (square / 8 < 6 && square % 8 < 7) {
            knight[square].reach |= 1ULL << (square + 17); // NNE.
            knight[square].block_bits.push_back(square + 17);
        }
        if (square / 8 < 7 && square % 8 < 6) {
            knight[square].reach |= 1ULL << (square + 10); // ENE.
            knight[square].block_bits.push_back(square + 10);
        }
        if (square / 8 > 0 && square % 8 < 6) {
            knight[square].reach |= 1ULL << (square - 6); // ESE.
            knight[square].block_bits.push_back(square - 6);
        }
        if (square / 8 > 1 && square % 8 < 7) {
            knight[square].reach |= 1ULL << (square - 15); // SES.
            knight[square].block_bits.push_back(square - 15);
        }
        if (square / 8 > 1 && square % 8 > 0) {
            knight[square].reach |= 1ULL << (square - 17); // SWS.
            knight[square].block_bits.push_back(square - 17);
        }
        if (square / 8 > 0 && square % 8 > 1) {
            knight[square].reach |= 1ULL << (square - 10); // ESE.
            knight[square].block_bits.push_back(square - 10);
        }
        if (square / 8 < 7 && square % 8 > 1) {
            knight[square].reach |= 1ULL << (square + 6); // WNW.
            knight[square].block_bits.push_back(square + 6);
        }
        if (square / 8 < 6 && square % 8 > 0) {
            knight[square].reach |= 1ULL << (square + 15); // NWN.
            knight[square].block_bits.push_back(square + 15);
        }

        // Sort the block bits.
        std::sort(knight[square].block_bits.begin(), knight[square].block_bits.end());

        // Set the moves.
        std::vector<int>* blockers = &(knight[square].block_bits);
        knight[square].move_set.resize(std::pow(2, blockers->size()));

        // Iterate over end occupancies.
        for (int i = 0; i < std::pow(2, blockers->size()); i++) {
            uint64_t pos = knight[square].reach;
        
            // Create the position with masked ends.
            int index = 0;
            for (auto j = blockers->begin(); j != blockers->end(); j++) {
                pos = (pos & ~(1ULL << *j)) | ((1ULL & i >> index) << *j);
                index++;
            }

            std::vector<Move>* moves_set = &(knight[square].move_set[moveSetIndex(pos, &knight[square])]);
            
            // Set the moves.
            for (uint64_t shift = 0; shift < 64; shift++) {
                if (((pos >> shift) & 1ULL) == 1) {
                    moves_set->push_back(square | shift << 6 | NORMAL | pKNIGHT);
                }
            }
        }
    }
    return knight;
}

/**
 * Computes pawn moves.
 * @param PAWN_MOVES: The array of pawn moves structs.
 * @param EN_PASSANT_MOVES: Array of en-passant moves structs. 32 is the offset
 *  to index a moves struct. move_set index of 1 is for left ep, 0 is for right
 *  ep.
 * @param DOUBLE_PUSH: An array of moves structs for double pawn push blocks.
 */
std::vector<std::vector<MovesStruct>> computePawnMoves() {
    std::vector<std::vector<MovesStruct>> PAWN_MOVES = {std::vector<MovesStruct>(48), std::vector<MovesStruct>(48)};
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
                PAWN_MOVES[player][square - 8].reach |= 1ULL << (square + left);
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + left);
                pos |= 1ULL << (square + left);
            }
            if (square % 8 != 7) {
                PAWN_MOVES[player][square - 8].reach |= 1ULL << (square + right);
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + right);
                pos |= 1ULL << (square + right);
            }

            // Add one push block bit.
            PAWN_MOVES[player][square - 8].block_bits.push_back(square + forward_push);
            pos |= 1ULL << (square + forward_push);

            // Add two push block bit.
            if ((player == WHITE && square / 8 == 1) || (player == BLACK && square / 8 == 6)) {
                PAWN_MOVES[player][square - 8].block_bits.push_back(square + double_push);
                pos |= 1ULL << (square + double_push);
            }
            
            // Sort the block bits.
            std::sort(PAWN_MOVES[player][square - 8].block_bits.begin(), PAWN_MOVES[player][square - 8].block_bits
                    .end());

            // Set the moves.
            std::vector<int>* blockers = &(PAWN_MOVES[player][square - 8].block_bits);
            PAWN_MOVES[player][square - 8].move_set.resize(std::pow(2, blockers->size()));

            // Iterate over occupancies.
            for (int i = 0; i < std::pow(2, blockers->size()); i++) {
                // Create the position with masked ends.
                int index = 0;
                for (auto j = blockers->begin(); j != blockers->end(); j++) {
                    pos = (pos & ~(1ULL << *j)) | ((1ULL & (i >> index)) << *j);
                    index++;
                }

                std::vector<Move>* moves_set = &(PAWN_MOVES[player][square -8].move_set[moveSetIndex(pos,
                        &PAWN_MOVES[player][square - 8])]);

                if (player == WHITE) {
                    if (square % 8 != 7 && square / 8 == 6) { // Right promo.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 6 | PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + right) << 6 | PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + right) << 6 | PROMOTION | pROOK);
                            moves_set->push_back(square | (square + right) << 6 | PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 7) { // Normal right capture.
                        if (bitAt(pos, square + right)) { // Right capture.
                            moves_set->push_back(square | (square + right) << 6 | NORMAL | pKNIGHT);
                        }
                    }

                    if (square % 8 != 0 && square / 8 == 6) { // Left promo
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 | PROMOTION | pKNIGHT);
                            moves_set->push_back(square | (square + left) << 6 | PROMOTION | pBISHOP);
                            moves_set->push_back(square | (square + left) << 6 | PROMOTION | pROOK);
                            moves_set->push_back(square | (square + left) << 6 | PROMOTION | pQUEEN);
                        }
                    } else if (square % 8 != 0) { // Normal left capture.
                        if (bitAt(pos, square + left)) { // Left capture.
                            moves_set->push_back(square | (square + left) << 6 | NORMAL | pKNIGHT);
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
    return PAWN_MOVES;
}

std::vector<MovesStruct> computeEnPassantMoves() {
    std::vector<MovesStruct> moves(16);
    for (int player = BLACK; player <= WHITE; player++) {
        int left = player == BLACK ? -9 : 7;
        int right = player == BLACK ? -7 : 9;
        
        for (int square = 8; square <= 55; square++) {
            // En-passant captures
            if (player == WHITE && square / 8 == 4) { // En-passant.
                moves[square - 24].move_set.resize(2);
                std::vector<Move>* r_ep_set = &(moves[square - 24].move_set[0]);
                std::vector<Move>* l_ep_set = &(moves[square - 24].move_set[1]);
                r_ep_set->push_back(square | (square + right) << 6 | EN_PASSANT | pKNIGHT);
                l_ep_set->push_back(square | (square + left) << 6 | EN_PASSANT |pKNIGHT);
            } else if (player == BLACK && square / 8 == 3) { // En-passant.
                moves[square - 24].move_set.resize(2);
                std::vector<Move>* r_ep_set = &(moves[square - 24].move_set[0]);
                std::vector<Move>* l_ep_set = &(moves[square - 24].move_set[1]);
                r_ep_set->push_back(square | (square + right) << 6 | EN_PASSANT | pKNIGHT);
                l_ep_set->push_back(square | (square + left) << 6 | EN_PASSANT |pKNIGHT);
            }
        }
    }
    return moves;
}

std::vector<MovesStruct> computeDoublePushMoves() {
    std::vector<MovesStruct> DOUBLE_PUSH(16);
    for (int player = BLACK; player <= WHITE; player++) {        
        for (int square = 8; square <= 55; square++) {
            // Double push blocks
            if (player == WHITE && square / 8 == 1) {
                DOUBLE_PUSH[square - 8].move_set.resize(1);
                std::vector<Move>* move = &(DOUBLE_PUSH[square - 8].move_set[0]);
                move->push_back(square | (square + 16) << 6 | NORMAL | pKNIGHT);
            } else if (player == BLACK && square / 8 == 6) {
                DOUBLE_PUSH[square - 40].move_set.resize(1);
                std::vector<Move>* move = &(DOUBLE_PUSH[square - 40].move_set[0]);
                move->push_back(square | (square - 16) << 6 | NORMAL | pKNIGHT);
            }
        }
    }
    return DOUBLE_PUSH;
}

/**
 * Sets the moves of the rook for a particular square and occupancy (according
 * to MSBs) and creates the moves. This only happens if the family of moves has
 * not been set yet.
 * @param square: The square that the rook is on.
 * @param move_family: The struct holding the moves family for rooks.
 */
void setRookMoves(int square, MovesStruct* move_family) {
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

        std::vector<Move>* moves_set = &(move_family->
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
void computeRCornerMoves(int square, int* offset, std::vector<MovesStruct>* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    for (int vOcc = 0; vOcc < 64; vOcc++) {
        for (int hOcc = 0; hOcc < 64; hOcc++) {
            // Build position.
            int index;
            Bitboard pos = 0;
            
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

            // Index creation
            int move_index = MSB(hOcc) + 7 * MSB(vOcc) + *offset;

            if ((*ROOK_MOVES)[move_index].reach == UNSET) {
                (*ROOK_MOVES)[move_index].block_bits.push_back(
                    square / 8 == 0 ? square + 8 * (7 - MSB(vOcc)) : square - 8 * (7 - MSB(vOcc))
                );
                (*ROOK_MOVES)[move_index].block_bits.push_back(
                    square % 8 == 0 ? square + (7 - MSB(hOcc)) : square - (7 - MSB(hOcc))
                );
                std::sort((*ROOK_MOVES)[move_index].block_bits.begin(), (*ROOK_MOVES)[move_index].block_bits.end());
                setRookMoves(square, &((*ROOK_MOVES)[move_index]));
            }
        }
    }

    *offset += 49;
}

std::vector<int> computeRCornerIndices(int square, int* offset) {
    std::vector<int> indices;

    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    indices.resize(4096);

    for (int vOcc = 0; vOcc < 64; vOcc++) {
        for (int hOcc = 0; hOcc < 64; hOcc++) {
            // Build position.
            int index;
            Bitboard pos = 0;
            
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
                ((pos & rookMasks[square]) * rookMagicNums[square]) >> rookShifts[square]
            );

            int move_index = MSB(hOcc) + 7 * MSB(vOcc) + *offset;
            indices[rookIndex] = move_index;
        }
    }

    *offset += 49;

    return indices;
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
void computeRLRSideMoves(int square, int* offset, std::vector<MovesStruct>* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

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
                int move_index;
                if (square / 8 == 1) { // A2 H2
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + *offset;
                    offset_add = 42;
                } else if (square / 8 == 2) { // A3 H3
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 14 * MSB(vOcc2) + *offset;
                    offset_add = 70;
                } else if (square / 8 == 3) { // A4 H4
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 21 * MSB(vOcc2) + *offset;
                    offset_add = 84;
                } else if (square / 8 == 4) { // A5 H5
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 21 * MSB(vOcc1) + *offset;
                    offset_add = 84;
                } else if (square / 8 == 5) { // A6 H6
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 14 * MSB(vOcc1) + *offset;
                    offset_add = 70;
                } else { // A7 H7
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + *offset;
                    offset_add = 42;
                }
                
                if ((*ROOK_MOVES)[move_index].reach == UNSET) {
                    (*ROOK_MOVES)[move_index].block_bits.push_back(
                        square + 8 * ((7 - square / 8) - MSB(vOcc2))
                    );
                    (*ROOK_MOVES)[move_index].block_bits.push_back(
                        square - 8 * ((square / 8) - MSB(vOcc1))
                    );
                    (*ROOK_MOVES)[move_index].block_bits.push_back(
                        square % 8 == 0 ? square + (7 - MSB(hOcc)) :
                                square - (7 - MSB(hOcc))
                    );
                    std::sort((*ROOK_MOVES)[move_index].block_bits.begin(),
                            (*ROOK_MOVES)[move_index].block_bits.end());
                    setRookMoves(square, &((*ROOK_MOVES)[move_index]));
                }
            }
        }
    }

    *offset += offset_add;
}

std::vector<int> computeRLRSideIndices(int square, int* offset) {
    std::vector<int> indices;

    // The horizontal and vertical start and end squares.
    int hStart = (square / 8) * 8 + 1;
    int hEnd = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

    indices.resize(2048);

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
                uint64_t rookIndex = ((pos & rookMasks[square]) * rookMagicNums[square]) >> rookShifts[square];

                int move_index;
                if (square / 8 == 1) { // A2 H2
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 42;
                } else if (square / 8 == 2) { // A3 H3
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 14 * MSB(vOcc2) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 70;
                } else if (square / 8 == 3) { // A4 H4
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + 21 * MSB(vOcc2) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 84;
                } else if (square / 8 == 4) { // A5 H5
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 21 * MSB(vOcc1) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 84;
                } else if (square / 8 == 5) { // A6 H6
                    move_index = MSB(hOcc) + 7 * MSB(vOcc2) + 14 * MSB(vOcc1) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 70;
                } else { // A7 H7
                    move_index = MSB(hOcc) + 7 * MSB(vOcc1) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 42;
                }
            }
        }
    }

    *offset += offset_add;
    return indices;
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
void computeRULSideMoves(int square, int* offset, std::vector<MovesStruct>* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

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
                int move_index;
                if (square % 8 == 1) { // B1 B8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + *offset;
                    offset_add = 42;
                } else if (square % 8 == 2) { // C1 C8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 14 * MSB(hOcc2) + *offset;
                    offset_add = 70;
                } else if (square % 8 == 3) { // D1 D8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 21 * MSB(hOcc2) + *offset;
                    offset_add = 84;
                } else if (square % 8 == 4) { // E1 E8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 21 * MSB(hOcc1) + *offset;
                    offset_add = 84;
                } else if (square % 8 == 5) { // F1 F8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 14 * MSB(hOcc1) + *offset;
                    offset_add = 70;
                } else { // G1 G8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + *offset;
                    offset_add = 42;
                }

                if ((*ROOK_MOVES)[move_index].reach == UNSET) {
                    (*ROOK_MOVES)[move_index].block_bits.push_back(
                        square / 8 == 0 ? square + 8 * (7 - MSB(vOcc)) :
                            square - 8 * (7 - MSB(vOcc))
                    );
                    (*ROOK_MOVES)[move_index].block_bits.push_back(
                        square - ((square % 8) - MSB(hOcc1))
                    );
                    (*ROOK_MOVES)[move_index].block_bits.push_back(
                        square + (7 - square % 8) - MSB(hOcc2)
                    );
                    std::sort((*ROOK_MOVES)[move_index].block_bits.begin(),
                            (*ROOK_MOVES)[move_index].block_bits.end());
                    setRookMoves(square, &((*ROOK_MOVES)[move_index]));
                }
            }
        }
    }

    *offset += offset_add;
}

std::vector<int> computeRULSideIndices(int square, int* offset) {
    std::vector<int> indices;

    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int vStart = (square % 8) + 8;
    int vEnd = (square % 8) + 48;

    indices.resize(2048);

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
                    indices[rookIndex] = move_index;
                    offset_add = 42;
                } else if (square % 8 == 2) { // C1 C8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 14 * MSB(hOcc2) + 
                            *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 70;
                } else if (square % 8 == 3) { // D1 D8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + 21 * MSB(hOcc2) + 
                            *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 84;
                } else if (square % 8 == 4) { // E1 E8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 21 * MSB(hOcc1) + 
                            *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 84;
                } else if (square % 8 == 5) { // F1 F8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc2) + 14 * MSB(hOcc1) + 
                            *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 70;
                } else { // G1 G8
                    move_index = MSB(vOcc) + 7 * MSB(hOcc1) + *offset;
                    indices[rookIndex] = move_index;
                    offset_add = 42;
                }
            }
        }
    }

    *offset += offset_add;
    return indices;
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
void computeRCentreMoves(int square, int* offset, std::vector<MovesStruct>* ROOK_MOVES) {
    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

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
                    int move_index;
                    if (square == B2 || square == B7 || square == G2 || square == G7) {
                        move_index = MSB((h1_occs != 1 ? hOcc1 : hOcc2)) + 6 * MSB((v1_occs != 1 ? vOcc1 : vOcc2)) + 
                                *offset;
                        offset_add = 36;
                    } else if (square == C2 || square == F2 || square == C7 || square == F7 || square == B3 ||
                            square == G3 || square == B6 || square == G6) {
                        move_index = MSB(h1_occs == 32 ? hOcc1 : (h2_occs == 32 ? hOcc2 : (v1_occs == 32 ? vOcc1 :
                                vOcc2))) + 6 * MSB(h1_occs == 2 ? hOcc1 : (h2_occs == 2 ? hOcc2 : (v1_occs == 2 ?
                                vOcc1 : vOcc2))) + 12 * MSB(h1_occs == 16 ? hOcc1 : (h2_occs == 16 ? hOcc2 :
                                (v1_occs == 16 ? vOcc1 : vOcc2))) +*offset;
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
                        offset_add = 72;
                    } else if (square == C3 || square == F3 || square == C6 || 
                            square == F6) {
                        move_index = MSB(h1_occs == 16 ? hOcc1 : hOcc2) +
                                25 * MSB(h1_occs == 2 ? hOcc1 : hOcc2) +
                                5 * MSB(v1_occs == 16 ? vOcc1 : vOcc2) +
                                50 * MSB(v1_occs == 2 ? vOcc1 : vOcc2) +
                                *offset;
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
                        offset_add = 120;
                    } else {
                        move_index = MSB(h1_occs == 8 ? hOcc1 : hOcc2) +
                                4 * MSB(v1_occs == 8 ? vOcc1 : vOcc2) +
                                16 * MSB(v1_occs == 4 ? vOcc1 : vOcc2) +
                                48 * MSB(h1_occs == 4 ? hOcc1 : hOcc2) +
                                *offset;
                        offset_add = 144;
                    }

                    if ((*ROOK_MOVES)[move_index].reach == UNSET) {
                        (*ROOK_MOVES)[move_index].block_bits.push_back(
                            square - 8 * ((square / 8) - MSB(vOcc1))
                        );
                        (*ROOK_MOVES)[move_index].block_bits.push_back(
                            square - ((square % 8) - MSB(hOcc1))
                        );
                        (*ROOK_MOVES)[move_index].block_bits.push_back(
                            square + (7 - square % 8) - MSB(hOcc2)
                        );
                        (*ROOK_MOVES)[move_index].block_bits.push_back(
                            square + 8 * ((7 - square / 8) - MSB(vOcc2))
                        );

                        std::sort((*ROOK_MOVES)[move_index].block_bits.begin(),
                                (*ROOK_MOVES)[move_index].block_bits.end());
                        setRookMoves(square, &((*ROOK_MOVES)[move_index]));
                    }
                }
            }
        }
    }

    *offset += offset_add;
}

std::vector<int> computeRCentreIndices(int square, int* offset) {
    std::vector<int> indices;

    // The horizontal and vertical start and end squares.
    int h1_Start = (square / 8) * 8 + 1;
    int h1_End = square - 1;
    int h2_Start = square + 1;
    int h2_End = (square / 8) * 8 + 6;
    int v1_Start = (square % 8) + 8;
    int v1_End = square - 8;
    int v2_Start = square + 8;
    int v2_End = (square % 8) + 48;

    indices.resize(1024);

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
                    uint64_t rookIndex = ((pos & rookMasks[square]) * rookMagicNums[square]) >> rookShifts[square];

                    int move_index;
                    if (square == B2 || square == B7 || square == G2 || 
                            square == G7) {
                        move_index = MSB((h1_occs != 1 ? hOcc1 : hOcc2)) + 
                                6 * MSB((v1_occs != 1 ? vOcc1 : vOcc2)) + 
                                *offset;
                        indices[rookIndex] = move_index;
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
                        indices[rookIndex] = move_index;
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
                        indices[rookIndex] = move_index;
                        offset_add = 72;
                    } else if (square == C3 || square == F3 || square == C6 || 
                            square == F6) {
                        move_index = MSB(h1_occs == 16 ? hOcc1 : hOcc2) +
                                25 * MSB(h1_occs == 2 ? hOcc1 : hOcc2) +
                                5 * MSB(v1_occs == 16 ? vOcc1 : vOcc2) +
                                50 * MSB(v1_occs == 2 ? vOcc1 : vOcc2) +
                                *offset;
                        indices[rookIndex] = move_index;
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
                        indices[rookIndex] = move_index;
                        offset_add = 120;
                    } else {
                        move_index = MSB(h1_occs == 8 ? hOcc1 : hOcc2) +
                                4 * MSB(v1_occs == 8 ? vOcc1 : vOcc2) +
                                16 * MSB(v1_occs == 4 ? vOcc1 : vOcc2) +
                                48 * MSB(h1_occs == 4 ? hOcc1 : hOcc2) +
                                *offset;
                        indices[rookIndex] = move_index;
                        offset_add = 144;
                    }
                }
            }
        }
    }

    *offset += offset_add;
    return indices;
}

/**
 * Returns the index from ROOK_INDEX into ROOK_MOVES based on the square and 
 * occupancy.
 * @param pos: A bitboard of the pieces on the board.
 * @param square: The square of the piece whose moves we want.
 * @return: Index into ROOK_MOVES.
 */
const int rookIndex(const uint64_t pos, Square square) {
    return ((pos & rookMasks[square]) * rookMagicNums[square]) >> rookShifts[square];
}

/**
 * Iterates through the squares and call functions to compute rook moves and
 * calculate indices.
 * @param ROOK_INDEX: Array of vectors holding ints where indices into the rook
 *      attack sets are set.
 * @param ROOK_MOVES: Move struct to store the moves.
 */
std::vector<MovesStruct> computeRookMoves() {
    std::vector<MovesStruct> ROOK_MOVES(4900);

    // Set initial reach to UNSET.
    for (int i = 0; i < 4900; i++) {
        ROOK_MOVES[i].reach = UNSET;
    }

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            computeRCornerMoves(square, &offset, &ROOK_MOVES);
        } else if (square % 8 == 0 || square % 8 == 7) {
            computeRLRSideMoves(square, &offset, &ROOK_MOVES);
        } else if (square / 8 == 0 || square / 8 == 7) {
            computeRULSideMoves(square, &offset, &ROOK_MOVES);
        } else {
            computeRCentreMoves(square, &offset, &ROOK_MOVES);
        }
    }

    return ROOK_MOVES;
}

/**
 * Iterates through the squares and call functions to compute rook moves indices.
 */
std::vector<std::vector<int>> computeRookIndices() {
    std::vector<std::vector<int>> indices(64);

    // Set the moves and reach.
    int offset = 0;
    for (int square = A1; square <= H8; square++) {
        if (square == A1 || square == A8 || square == H1 || square == H8) {
            indices[square] = computeRCornerIndices(square, &offset);
        } else if (square % 8 == 0 || square % 8 == 7) {
            indices[square] = computeRLRSideIndices(square, &offset);
        } else if (square / 8 == 0 || square / 8 == 7) {
            indices[square] = computeRULSideIndices(square, &offset);
        } else {
            indices[square] = computeRCentreIndices(square, &offset);
        }
    }

    return indices;
}

/**
 * Computes the blocking moves (and captures in case of check) for rooks on each
 * square.
 * @param ROOK_BLOCKS: A vector of move structs.
 */
std::vector<MovesStruct> computeRookBlocks() {
    std::vector<MovesStruct> blocks(64);
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

            std::unordered_map<uint64_t, std::vector<Move>>* moves = 
                    &blocks[square].checked_moves;
            while (start != end_pt) {
                // moves[pos] = std::vector<Move>(); WHY DOESN'T WORK????
                Move move = square | start << 6 | NORMAL | pKNIGHT;
                std::vector<Move> moveset = {move};
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
            
            std::unordered_map<uint64_t, std::vector<Move>>* moves = 
                    &blocks[square].checked_moves;

            while (start1 != end1) {
                start2 = square + ray2;
                while (start2 != end2) {
                    Move move1 = square | start1 << 6 | NORMAL | pKNIGHT;
                    Move move2 = square | start2 << 6 | NORMAL | pKNIGHT;
                    std::vector<Move> moveset = {move1, move2};
                    moves->insert(std::make_pair(1ULL << start1 | 1ULL << start2, 
                            moveset));
                    start2 += ray2;
                }
                start1 += ray1;
            };
        };
    }

    return blocks;
}
