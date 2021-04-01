
#ifndef MCTS_HPP
#define MCTS_HPP

#include "game.hpp"

typedef Move Edge;

class Node {
    public:
        Node(Pos&, bool = false);

        std::vector<std::pair<Node, Edge>> parents;
        std::vector<std::pair<Node, Edge>> children;
        bool isRoot;
        float value = 0;
        int visits = 0;
        static int totalVisits;
        Hash hash;

        Node& select(Pos&, std::stack<Move>&);
        Node& expand();
        float simulate();
        void rollback();
        float UCB1() const;
};

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop);

#endif
