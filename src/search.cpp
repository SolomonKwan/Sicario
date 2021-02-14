
#include "game.hpp"
#include "search.hpp"

#include <iostream>
#include <cmath>
#include <vector>
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
    std::cout << "PV table initialised for " << (hashSize * 1000000) / sizeof(PV) << " entries\n";
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
void Pos::search(SearchParams params, MoveList& moves) {
    std::vector<Move> movesToConsider = this->orderMoves(params, moves);
    this->mcst(movesToConsider, params);
}

/**
 * Order the moves by most "interesting/best" first and least "intereseting/worse" last (i.e. order by score in
 * descending order).
 * 
 * TODO: Need to make the ordering algorithm cleaner. Somekind of ordering while iterating the first time.
 */
std::vector<Move> Pos::orderMoves(SearchParams, MoveList& moves) {
    std::vector<std::pair<int, Move>> ordering;
    for (Move move : moves) {
        ordering.push_back(std::make_pair(this->scoreMove(move), move));
    }
    std::sort(ordering.begin(), ordering.end());

    std::vector<Move> chosenMoves;
    for (std::pair<int, Move> move : ordering) {
        chosenMoves.push_back(move.second);
    }

    return chosenMoves;
}

/**
 * Gives a guess of the score of a move.
 * @param move: Move to guess the score of.
 * @return: Returns a positive integer indicating the score of the move. The higher the score, the more preferred the
 * move.
 */
int Pos::scoreMove(Move move) {
    int score = 0;
    // PV move
    score += this->kingSafety(move); // King safety
    score += this->captures(move);// Captures
    // Checks
    // Passed pawns
    // X-rays
    // Mobility
    // PSQT
    return score;
}

/**
 * Estimate the safety of the king.
 * @param move: Move to guess the score of.
 */
int Pos::kingSafety(Move move) {
    int score = 0;
    return score;
}

/**
 * Gives a preferential score to certain captures first.
 */
int Pos::captures(Move move) {
    int score = 0;
    int start = move & 0b111111, end = (move >> 6) & 0b111111;
    PieceType moved = this->pieces[start], captured = this->pieces[end];
    return score;
}

void Pos::mcst(std::vector<Move>, SearchParams) {
    
}
