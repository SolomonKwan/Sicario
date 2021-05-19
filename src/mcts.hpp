
#ifndef MCTS_HPP
#define MCTS_HPP

#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include "game.hpp"

typedef Move Edge;

struct Info {
    int depth = 0;
    int seldepth = 0;
    int multipv = 0;
    int cp = 0;
    int nodes = 0;
    int nps = 0;
    int tbhits = 0;
    int time = 0;
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

        Node* select(Pos&, SearchParams&);
        Node* expand(Pos&, std::unordered_map<Hash, std::unordered_set<Node*>>&, SearchParams&);
        float simulate(Pos&);
        void rollback(float, Pos&, std::unordered_map<Hash, std::unordered_set<Node*>>&);

        float UCB1(SearchParams&) const;
        Node* bestChild(SearchParams&);
};

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop);

#endif
