
#ifndef MCTS_HPP
#define MCTS_HPP

#include <atomic>

#include "game.hpp"
#include "constants.hpp"

const float C = std::sqrt(2); // TODO make this a uci paramter for user to set.

typedef Move Edge;

class Node {
    public:
        Node(Move move, Node* parent);

        inline void addChild(Move move);

        inline float UCB1() const {
            return visits == 0 ? std::numeric_limits<float>::min() : (value / visits) + C *
                    std::sqrt(std::log(parent->visits) / visits);
        }

        void freeChildren();

        Node* select(Position& position);

        Node* expand(Position& position);

        float simulate(Position& position, std::atomic_bool& searchTree, Player rootPlayer);

        void rollback(Position& position, float val, std::atomic_bool& searchTree, Player rootPlayer);

        inline Move getInEdge() const {
            return inEdge;
        }

        struct UCB1Comparator {
            bool operator ()(const Node* a, const Node* b) const {
                return a->UCB1() < b->UCB1();
            }
        };

    // private:
        Edge inEdge;
        Node* parent = nullptr; // TODO make a node have multiple parents
        std::vector<Node*> children;
        float value = 0;
        float visits = 0;

        // bool is_root;
        // Hash hash;
        // Move incoming_move;
        // std::vector<Node*> children;
        // static Player rootPlayer;
        // bool turn;
        // int depth;


        // Node* select();
        // Node* expand();
        // float simulate();
        // void rollback();

        // float UCB1() const;
        // Node* bestChild();
};

class Searcher {
    public:
        Searcher(Position position);
        void search(std::atomic_bool& searchTree);

        Position position;
        const Player rootPlayer;
};

#endif
