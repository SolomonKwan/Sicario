
#include "game.hpp"
#include "search.hpp"

#include <iostream>
#include <cmath>

/**
 * @param hashSize: Hash table size in megabytes.
 */
SearchInfo::SearchInfo(int hashSize) {
    this->PV_table.reserve((hashSize * 1000000) / sizeof(PV));
}

/**
 * @param hashSize: Hash table size in megabytes.
 */
void SearchInfo::setHashSize(int hashSize) {
    this->originalSize = hashSize;
    this->PV_table.reserve((hashSize * 1000000) / sizeof(PV));
    std::cout << "PV table initialised for " << (hashSize * 1000000) / sizeof(PV) << " entries\n";
}

/**
 * Clears the PV table but keeps the original reserve size same.
 */
void SearchInfo::clearTable() {
    this->PV_table.clear();
    this->PV_table.reserve((this->originalSize * 1000000) / sizeof(PV));
}

float Pos::alphaBeta(int depth, double alpha, double beta, bool max) {
    // Get moves
    std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS];
    int moves_index;
    this->getMoves(moves_index, pos_moves);

    ExitCode code = isEOG(moves_index);
    if (code == WHITE_WINS) {
        return 10000;
    } else if (code == BLACK_WINS) {
        return -10000;
    } else if (code) {
        return 0;
    }

    // Perform search
    if (depth == 0) {
        Evaluator evaluator(*this);
        return evaluator.evaluate();
    }

    if (max) {
        double value = -100000;
        for (int i = 0; i < moves_index; i++) {
            std::vector<uint16_t>* move_set = pos_moves[i];
            for (uint16_t move : *move_set) {
                this->makeMove(move);
                value = std::fmax(value, alphaBeta(depth - 1, alpha, beta, false));
                this->undoMove();
                alpha = std::fmax(alpha, value);
                if (alpha >= beta) break;
            }
        }
        return value;
    } else {
        double value = 100000;
        for (int i = 0; i < moves_index; i++) {
            std::vector<uint16_t>* move_set = pos_moves[i];
            for (uint16_t move : *move_set) {
                this->makeMove(move);
                value = std::fmin(value, alphaBeta(depth - 1, alpha, beta, true));
                this->undoMove();
                beta = std::fmin(beta, value);
                if (beta <= alpha) break;
            }
        }
        return value;
    }
}

Move Pos::search(std::vector<Move>* pos_moves[MAX_MOVE_SETS], int& moves_index) {
    double value = this->turn ? -1000000 : 1000000;
    std::cout << "Thinking..." << std::flush;
    Move move;
    for (int i = 0; i < moves_index; i++) {
        std::vector<Move>* move_set = pos_moves[i];
        for (Move move_candidate : *move_set) {
            this->makeMove(move_candidate);
            double pos_value = this->alphaBeta(this->depth, -100000, 100000, this->turn);
            if (pos_value > value && !this->turn) {
                move = move_candidate;
                value = pos_value;
            } else if (pos_value < value && this->turn) {
                move = move_candidate;
                value = pos_value;
            }
            this->undoMove();
        }
    }
    std::cout << "\rComputer move: ";
    printMove(move, true);
    return move;
}
