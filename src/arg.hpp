
#ifndef ARG_HPP
#define ARG_HPP

#include "game.hpp"

namespace Args {
    void init(Pos* game, int argc, char *argv[], CmdLine* args);
}

/**
 * Parse the command line arguments.
 * 
 * @param: game The Pos struct representing the game position.
 * @param: argc The argument count.
 * @param: argv The array of command line arguments.
 */
ExitCode parseArgs(Pos* game, int argc, char *argv[], CmdLine* args);

#endif
