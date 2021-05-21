
#include "game.hpp"
#include "search.hpp"
#include "mcts.hpp"
#include "uci.hpp"

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <atomic>

namespace Moves {
    extern std::vector<MovesStruct> ROOK;
    extern std::vector<MovesStruct> BISHOP;
    extern std::vector<MovesStruct> KNIGHT;
    extern std::vector<MovesStruct> KING;
    extern std::vector<MovesStruct> CASTLING;
    extern std::vector<MovesStruct> EN_PASSANT;
    extern std::vector<MovesStruct> DOUBLE_PUSH;
    extern std::vector<std::vector<MovesStruct>> PAWN;

    namespace Blocks {
        extern std::vector<MovesStruct> BISHOP;
        extern std::vector<MovesStruct> ROOK;
    }
}

/**
 * Vector of indices for rook and bishop indexing.
 */
namespace Indices {
    extern const std::vector<std::vector<int>> ROOK;
    extern const std::vector<std::vector<int>> BISHOP;
}

/**
 * Rays bitboards from start (exlusive) to end (inclusive).
 */
namespace Rays {
    extern const std::vector<std::vector<Bitboard>> LEVEL;
    extern const std::vector<std::vector<Bitboard>> DIAGONAL;
}

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
void Pos::search(SearchParams params, std::atomic_bool& stop, GoParams go_params) {
    Searcher searcher = Searcher(*this, params);
    searcher.mcts(stop);
}

Searcher::Searcher(Pos pos, SearchParams searchParams) {
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
std::vector<std::pair<int, Move>> Pos::scoreMoves(SearchParams, MoveList& moves) {
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
int Pos::scoreMove(Move move) {
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
int Pos::kingSafety(Move move) {
    return 0;
}

int Pos::scoreCastlingSafety(Move move) {
    return 0;
}

int Pos::scoreKingSafety(Move move) {
    return 0;
}

/**
 * Gives a preferential score to certain captures first.
 */
int Pos::captures(Move move) {
    return 0;
}
