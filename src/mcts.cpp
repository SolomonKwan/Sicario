
#include<unistd.h>

#include <iostream>
#include "mcts.hpp"

void Pos::mcst(std::vector<std::pair<int, Move>> scores, SearchParams sp, std::atomic_bool& stop) {
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
