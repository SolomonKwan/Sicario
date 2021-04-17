
#ifndef MCTS_HPP
#define MCTS_HPP

#include <stack>
#include <cmath>
#include "game.hpp"

typedef Move Edge;

class Node {
    public:
        Node(Move, bool = false, Hash = 0ULL);

        bool isRoot;
        Hash hash;
        Move incoming_move;
        Node* parent;
        std::vector<Node*> children;

        float value = 0;
        float visits = 0;
        float c = std::sqrt(2);
        static float totalVisits;

        Node* select(Pos&);
        Node* expand(Pos&);
        float simulate(Pos&);
        void rollback(float, Pos&);

        float UCB1() const;
        static void resetTotalCount();
};

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop);

#endif
