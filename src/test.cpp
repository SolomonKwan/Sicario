
#include <iostream>
#include <chrono>

#include "game.hpp"
#include "test.hpp"
#include "fen.hpp"

/**
 * Prints the promotion moves for the leaf nodes.
 * @param move: The moves to print for.
 */
void printPromo(uint16_t move) {
    if ((move & (0b11 << 12)) == PROMOTION) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
            0b111111];
        if ((move & (0b11 << 14)) == pKNIGHT) {
            std::cout << "n: 1\n";
        } else if ((move & (0b11 << 14)) == pBISHOP) {
            std::cout << "b: 1\n";
        } else if ((move & (0b11 << 14)) == pROOK) {
            std::cout << "r: 1\n";
        } else {
            std::cout << "q: 1\n";
        }
    } else if ((move & (0b11 << 12)) == EN_PASSANT) {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
            0b111111] << ": 1\n";
    } else {
        std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
            0b111111] << ": 1\n";
    }
}

/**
 * Prints the perft move divide.
 * @param print: Boolean to indicate whether or not to print.
 * @param move: Move to print for.
 * @param current_nodes: Number of leaf nodes from playing move.
 */
void printPerft(bool print, uint16_t move, uint64_t* current_nodes) {
    if (print) {
        if ((move & (0b11 << 12)) == PROMOTION) {
            std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
                    0b111111];
            if ((move & (0b11 << 14)) == pKNIGHT) {
                std::cout << "n: " << *current_nodes << '\n';
            } else if ((move & (0b11 << 14)) == pBISHOP) {
                std::cout << "b: " << *current_nodes << '\n';
            } else if ((move & (0b11 << 14)) == pROOK) {
                std::cout << "r: " << *current_nodes << '\n';
            } else {
                std::cout << "q: " << *current_nodes << '\n';
            }
        } else if ((move & (0b11 << 12)) == EN_PASSANT) {
            std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
                    0b111111] << ": " << *current_nodes << '\n';
        } else {
            std::cout << squareName[move & 0b111111] << squareName[(move >> 6) &
                    0b111111] << ": " << *current_nodes << '\n';
        }
    }
}

/**
 * Runs perft testing.
 * @param depth: The depth to search to (not including current depth).
 * @param game: Pointer to game struct.
 * @param precomputed_moves: Struct of precomputed moves.
 * @param print: Boolean to indicate whether or not to print.
 * @param args: Pointer to command line arguments.
 */
uint64_t runPerft(int depth, Pos* game, Computed* precomputed_moves, 
        bool print) {
    uint64_t nodes = 0;
    uint64_t enemy_attacks = 0;
    std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS];
    int moves_index = game->getMoves(precomputed_moves, &enemy_attacks, 
            pos_moves);

    for (int i = 0; i < moves_index; i++) {
        std::vector<uint16_t>* move_set = pos_moves[i];
        for (uint16_t move : *move_set) {
            uint64_t current_nodes = 0;
            if (depth == 1) {
                nodes += move_set->size();
                if (print) {
                    for (uint16_t move2 : *move_set) {
                        printPromo(move2);
                    }
                }
                break;
            } else {
                game->makeMove(move);
                current_nodes = runPerft(depth - 1, game, 
                        precomputed_moves, false);
            }

            printPerft(print, move, &current_nodes);

            nodes += current_nodes;
            game->undoMove();
        }
    }
    
    return nodes;
}

/**
 * Prepares and makes call to run perft.
 * @param args: Pointer to command line arguments.
 * @param game: Pointer to struct of game.
 */
void perft(int depth, Pos* game) {
    if (depth != 0) {
        Computed computed_moves;
        precompute(&computed_moves);
        uint64_t num = runPerft(depth, game, &computed_moves, 
                true);
        std::cout << "\nNodes searched: " << num << "\n\n";
    }
}
