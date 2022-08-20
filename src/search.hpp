
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <atomic>
#include <chrono>

#include "game.hpp"
#include "constants.hpp"

const float C = std::sqrt(2); // TODO make this a uci paramter for user to set.

typedef Move Edge;

// GUI info command data
struct GuiInfo {
    uint64_t nodes = 0;
    uint hashfull = 0;
    uint64_t nps_nodeCount = 0;
};

class Searcher;

class Node {
    public:
        Node(Move move, Node* parent);

        inline void addChild(Move move, Searcher* searcher);

        inline float UCB1() const {
            return visits == 0 ? std::numeric_limits<float>::max() : (value / static_cast<float>(visits)) + C *
                    std::sqrt(std::log(static_cast<float>(parent->visits)) / static_cast<float>(visits));
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
        uint visits = 0;
};

class Searcher {
    public:
        Searcher(Position position, const std::atomic_bool& searchTree);
        void search();

        GuiInfo info;
        Position position;
        const Player rootPlayer;
        const std::atomic_bool& searchTree;
        std::chrono::_V2::system_clock::time_point lastMessage;

        friend Node* Node::select(Searcher* searcher);
        friend Node* Node::expand(Searcher* searcher);
        friend float Node::simulate(Searcher* searcher);
        friend void Node::rollback(Searcher* searcher, float val);

        void printInfo(std::unique_ptr<Node, std::default_delete<Node>>& root);
        void printBestMove(std::unique_ptr<Node, std::default_delete<Node>>& root);
};

#endif
