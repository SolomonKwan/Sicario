
#ifndef UCI_HPP
#define UCI_HPP

#include "constants.hpp"

namespace UCI {
    /**
     * Initialisation to start reading from stdin. Main function that starts
     * main thread of the program and handles all UCI interations.
     * @param game: Pointer to game struct.
     * @param moves: Pointer to precomputed moves struct.
     * @param args: Pointer to command line arguments.
     * @param argv: Command line arguments.
     */
    void init(Pos* game, Computed* moves, CmdLine* args, char *argv[], 
            std::string input);
}

#endif
