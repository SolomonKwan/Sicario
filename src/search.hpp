
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "constants.hpp"
#include "game.hpp"

class Searcher {
    public:
        Searcher();
    
    private:
        void search();
};

double evaluate(Pos* game);

double alphaBeta(Pos* game, int depth, double alpha, double beta, bool max, Computed* moves);

#endif
