
#include<unistd.h>

#include <iostream>
#include "mcts.hpp"

Node::Node(Pos& pos, bool root) : pos(pos) {
    this->isRoot = root;
}

void Pos::mcst(SearchParams sp, std::atomic_bool& stop) {
    MoveList moves(*this);
    std::vector<std::pair<int, Move>> ordered_moves = this->scoreMoves(sp, moves);
    while (!stop) {
        unsigned int microsecond = 1000000;
        std::cout << "\33[2K\rsearching   " << std::flush;
        usleep(0.5 * microsecond);
        std::cout << "\33[2K\rsearching.  " << std::flush;
        usleep(0.5 * microsecond);
        std::cout << "\33[2K\rsearching.. " << std::flush;
        usleep(0.5 * microsecond);
        std::cout << "\33[2K\rsearching..." << std::flush;
        usleep(0.5 * microsecond);
    }
}
