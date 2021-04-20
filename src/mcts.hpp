
#ifndef MCTS_HPP
#define MCTS_HPP

#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include "game.hpp"

typedef Move Edge;

class Node {
    public:
        Node(Move, bool, Hash, bool);

        bool is_root;
        Hash hash;
        Move incoming_move;
        Node* parent;
        std::vector<Node*> children;
        static Player rootPlayer;
        bool turn;

        float value = 0;
        float visits = 0;
        float c = std::sqrt(2);

        Node* select(Pos&);
        Node* expand(Pos&, std::unordered_map<Hash, std::unordered_set<Node*>>&);
        float simulate(Pos&);
        void rollback(float, Pos&, std::unordered_map<Hash, std::unordered_set<Node*>>&);

        float UCB1() const;
};

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop);

#endif
