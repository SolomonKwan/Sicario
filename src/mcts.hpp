
#ifndef MCTS_HPP
#define MCTS_HPP

#include "game.hpp"

class Node {
    public:
        Node(Pos&, bool root = false);

        Pos& pos;
        std::vector<Node> parents;
        std::vector<Node> children; // Ordered
        bool isRoot;
        float value;

        Node& select();
        void expand();
        float simulate();
        void rollback();
        float UCB1() const;
};

#endif
