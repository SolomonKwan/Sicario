
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "constants.hpp"

class Searcher {
    public:
        Searcher();
    
    private:
        void search();
};

double evaluate(Game* game);

double alphaBeta(Game* game, int depth, double alpha, double beta, bool max, Computed* moves);

#endif
