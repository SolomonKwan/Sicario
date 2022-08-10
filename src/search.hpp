
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <atomic>

#include "game.hpp"
#include "constants.hpp"

const float C = std::sqrt(2); // TODO make this a uci paramter for user to set.

typedef Move Edge;

class Searcher;

class Node {
    public:
        Node(Move move, Node* parent);

        inline void addChild(Move move);

        inline float UCB1() const {
            return visits == 0 ? std::numeric_limits<float>::max() : (value / visits) + C *
                    std::sqrt(std::log(parent->visits) / visits);
        }

        void freeChildren();

        Node* select(Searcher* searcher);

        Node* expand(Searcher* searcher);

        float simulate(Searcher* searcher);

        void rollback(Searcher* searcher, float val);

        inline Move getInEdge() const {
            return inEdge;
        }

        struct UCB1Comparator {
            bool operator ()(const Node* a, const Node* b) const {
                return a->UCB1() < b->UCB1();
            }
        };

        Edge inEdge;
        Node* parent = nullptr; // TODO make a node have multiple parents
        std::vector<Node*> children;
        float value = 0;
        float visits = 0;
};

class Searcher {
    public:
        Searcher(Position position, const std::atomic_bool& searchTree);
        void search();

        Position position;
        const Player rootPlayer;
        const std::atomic_bool& searchTree;

        friend Node* Node::select(Searcher* searcher);
        friend Node* Node::expand(Searcher* searcher);
        friend float Node::simulate(Searcher* searcher);
        friend void Node::rollback(Searcher* searcher, float val);
};

// GUI info command data
struct GuiInfo {
    uint nodes = 0;
    uint hasfull = 0;
    uint nps = 0;
};

#endif
