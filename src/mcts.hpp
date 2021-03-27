
#ifndef MCTS_HPP
#define MCTS_HPP

#include "game.hpp"

class Node {
    public:
        Node(Pos&, bool root = false);

        Node& select();
        void expand();
        float simulate();
        void rollback();
        float UCB1();

        Pos& pos;
        std::vector<Node> parents;
        std::vector<Node> children;
        bool isRoot;
};

#endif
