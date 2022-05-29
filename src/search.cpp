
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
void Position::search(SearchParams params, GoParams go_params) {
    Searcher searcher = Searcher(*this, params);
    searcher.mcts(go_params);
}

Searcher::Searcher(Position pos, SearchParams searchParams) {
    this->pos = pos;
    this->time = searchParams.time;
    this->children_to_search = searchParams.children_to_search;
    this->c = searchParams.c;
}

/**
 * Order the moves by most "interesting/best" first and least "intereseting/worse" last (i.e. order by score in
 * descending order).
 *
 * TODO: Need to make the ordering algorithm cleaner. Somekind of ordering while iterating the first time.
 */
std::vector<std::pair<int, Move>> Position::scoreMoves(SearchParams, MoveList& moves) {
    std::vector<std::pair<int, Move>> ordering;
    for (Move move : moves) {
        ordering.push_back(std::make_pair(this->scoreMove(move), move));
    }
    std::sort(ordering.begin(), ordering.end());
    return ordering;
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
