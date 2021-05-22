
#ifndef MCTS_HPP
#define MCTS_HPP

#include "search.hpp"
#include "constants.hpp"

typedef Move Edge;

struct Info {
    int depth = 0;
    int seldepth = 0;
    int multipv = 0;
    int cp = 0;
    int nodes = 0;
    int nps = 0;
    int tbhits = 0;
    std::chrono::_V2::system_clock::time_point time = std::chrono::high_resolution_clock::now();
    Move bestMove = 0U;
};

class Node {
    public:
        Node(Move, bool, Hash, bool, int);

        bool is_root;
        Hash hash;
        Move incoming_move;
        Node* parent;
        std::vector<Node*> children;
        static Player rootPlayer;
        bool turn;
        int depth;

        float value = 0;
        float visits = 0;

        Node* select(Searcher&);
        Node* expand(Searcher&);
        float simulate(Searcher&);
        void rollback(float, Searcher&);

        float UCB1(Searcher&) const;
        Node* bestChild(Searcher&);
};

#endif
