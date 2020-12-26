
#include <iostream>
#include <cmath>
#include "search.hpp"
#include "game.hpp"
#include "evaluate.hpp"
#include "movegen.hpp"

const double material_value[12] = {
    0, 0, 9, 5, 3.25, 3, 1, -9, -5, -3.25, -3, -1
};

double Pos::evaluate() {
    double value = 0;
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < this->piece_index[i]; j++) {
            if (Opening::is_phase(this)) {
                value += Opening::material_value[i];
                if (i == W_PAWN) {
                    value += Opening::w_pawn[this->piece_list[i][j]];
                } else if (i == B_PAWN) {
                    value += Opening::b_pawn[this->piece_list[i][j]];
                }
            } else if (MiddleGame::is_phase(this)) {
                value += MiddleGame::material_value[i];
                if (i == W_PAWN) {
                    value += MiddleGame::w_pawn[this->piece_list[i][j]];
                } else if (i == B_PAWN) {
                    value += MiddleGame::b_pawn[this->piece_list[i][j]];
                }
            } else {
                value += EndGame::material_value[i];
                if (i == W_PAWN) {
                    value += EndGame::w_pawn[this->piece_list[i][j]];
                } else if (i == B_PAWN) {
                    value += EndGame::b_pawn[this->piece_list[i][j]];
                }
            }
        }
    }
    return value;
}

Searcher::Searcher() {
    std::cout << "Searcher\n";
}

void Searcher::search() {

}

double Pos::alphaBeta(int depth, double alpha, double beta, bool max, Computed* moves) {
    // Get moves
    uint64_t enemy_attacks = 0;
    std::vector<uint16_t>* pos_moves[MAX_MOVE_SETS];
    int moves_index = this->getMoves(moves, &enemy_attacks, pos_moves);

    ExitCode code = this->isEOG(moves, enemy_attacks, moves_index);
    if (code == WHITE_WINS) {
        return 10000;
    } else if (code == BLACK_WINS) {
        return -10000;
    } else if (code) {
        return 0;
    }

    // Perform search
    if (depth == 0) {
        return this->evaluate();
    }

    if (max) {
        double value = -100000;
        for (int i = 0; i < moves_index; i++) {
            std::vector<uint16_t>* move_set = pos_moves[i];
            for (uint16_t move : *move_set) {
                this->makeMove(move);
                value = std::fmax(value, this->alphaBeta(depth - 1, alpha, beta, false, moves));
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
                value = std::fmin(value, this->alphaBeta(depth - 1, alpha, beta, true, moves));
                this->undoMove();
                beta = std::fmin(beta, value);
                if (beta <= alpha) break;
            }
        }
        return value;
    }
}
