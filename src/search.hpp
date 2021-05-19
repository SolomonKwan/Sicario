
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "constants.hpp"
#include "evaluate.hpp"
#include "movegen.hpp"
#include <unordered_map>
#include <cmath>

struct PV {
    Move move;
    float score;
};

struct SearchParams {
    int time = 5000; // 5 seconds for dev. Default shall be infinite.
    int children_to_search = 5; // Default to 5 child nodes for each node in the tree.
    float c = std::sqrt(2);
};

class SearchInfo {
    public:
        SearchInfo(int hashSize);
        void setHashSize(int hashSize);

    private:
        int originalSize = DEFAULT_HASH_SIZE;
        std::unordered_map<Hash, PV> PV_table;

        void clearTable();
};

#endif
