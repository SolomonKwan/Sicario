
// #include "game.hpp"
#include "search.hpp"
// #include "mcts.hpp"
#include "uci.hpp"

// #include <iostream>
// #include <cmath>
// #include <vector>
#include <algorithm>

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
    // std::cout << "PV table initialised for " << (hashSize * 1000000) / sizeof(PV) << " entries\n";
}

/**
 * Clears the PV table but keeps the original reserve size same.
 */
void SearchInfo::clearTable() {
    this->PV_table.clear();
    this->PV_table.reserve((this->originalSize * 1000000) / sizeof(PV));
}

/**
 * Search the current position.
 */
void Position::search(GoParams go_params) {
    Searcher searcher = Searcher(*this);
    searcher.mcts(go_params);
}

Searcher::Searcher(Position pos) {
    this->pos = pos;
}

/**
 * Gives a rank for the move.
 * @param move: Move to give the rank of.
 */
int Position::scoreMove(Move move) {
    ////////////////////// Modify below //////////////////////
    // PV move
    if (type(move) == CASTLING) return 2; // King safety
    if (this->pieces[end(move)] != NO_PIECE) return 3; // Captures
    if ((start(move) / 8 == 0 || start(move) / 8 == 7) && this->fullmove <= 10) return 4; // Developement
    else return 5;
    // Checks
    // Passed pawns
    // X-rays
    // Mobility
    // PSQT
    //////////////////////////////////////////////////////////
}

/**
 * Estimate the safety of the king.
 * @param move: Move to guess the score of.
 */
int Position::kingSafety(Move move) {
    return 0;
}

int Position::scoreCastlingSafety(Move move) {
    return 0;
}

int Position::scoreKingSafety(Move move) {
    return 0;
}

/**
 * Gives a preferential score to certain captures first.
 */
int Position::captures(Move move) {
    return 0;
}
