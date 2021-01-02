
#include "search.hpp"

#include <iostream>

/**
 * @param hashSize: Hash table size in megabytes.
 */
Searcher::Searcher(int hashSize) {
    this->PV_table.reserve((hashSize * 1000000) / sizeof(PV));
}

/**
 * @param hashSize: Hash table size in megabytes.
 */
void Searcher::setHashSize(int hashSize) {
    this->originalSize = hashSize;
    this->PV_table.reserve((hashSize * 1000000) / sizeof(PV));
    std::cout << "PV table initialised for " << (hashSize * 1000000) / sizeof(PV) << " entries\n";
}

/**
 * Clears the PV table but keeps the original reserve size same.
 */
void Searcher::clearTable() {
    this->PV_table.clear();
    this->PV_table.reserve((this->originalSize * 1000000) / sizeof(PV));
}
