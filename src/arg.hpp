
#ifndef ARG_HPP
#define ARG_HPP

namespace Args {
    void init(Game* game, int argc, char *argv[], CmdLine* args);
}

/**
 * Parse the command line arguments.
 * 
 * @param: game The Game struct representing the game position.
 * @param: argc The argument count.
 * @param: argv The array of command line arguments.
 */
ExitCode parseArgs(Game* game, int argc, char *argv[], CmdLine* args);

#endif
