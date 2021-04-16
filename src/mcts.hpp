
#ifndef MCTS_HPP
#define MCTS_HPP

#include <stack>
#include <cmath>
#include "game.hpp"

typedef Move Edge;

class Node {
    public:
        Node(bool = false);

        std::vector<std::pair<Node, Edge>> parents;
        std::vector<std::pair<Node, Edge>> children;
        bool isRoot;
        float value = 0;
        int visits = 0;
        float c = std::sqrt(2);
        static int totalVisits;

        Node& select(Pos&, std::stack<Move>&);
        Node& expand(Pos&, std::stack<Move>&, std::unordered_map<Hash, Node>&);
        float simulate(Pos&);
        void rollback(float, Pos&);
        float UCB1() const;
        static void resetTotalCount();
};

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop);

#endif
