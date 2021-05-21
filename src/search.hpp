
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "constants.hpp"
#include "evaluate.hpp"
#include "movegen.hpp"
#include "game.hpp"
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <chrono>

class Node;
class Searcher {
    public:
        Searcher(Pos pos, SearchParams params);
        Pos pos;
        void mcts(std::atomic_bool& stop);
        inline void incrementNodes() {
            this->nodes++;
        }

        // Search Parameters
        int time = 5000; // 5 seconds for dev. Default shall be infinite.
        int children_to_search = 5; // Default to 5 child nodes for each node in the tree.
        float c = std::sqrt(2);

        // Search info
        int depth = 0;
        int seldepth = 0;
        int multipv = 0;
        int cp = 0;
        int nodes = 0;
        int nps = 0;
        int tbhits = 0;
        std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
        Move bestMove = 0U;

        std::unordered_map<Hash, std::unordered_set<Node*>> hashPositions;
};

#endif
