
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "constants.hpp"
#include "movegen.hpp"
#include <unordered_map>

struct PV {
    Move move;
    float score;
};

class Searcher {
    public:
        Searcher(int hashSize);
        void setHashSize(int hashSize);

    private:
        int originalSize = DEFAULT_HASH_SIZE;
        std::unordered_map<Hash, PV> PV_table;

        void clearTable();
};

#endif
