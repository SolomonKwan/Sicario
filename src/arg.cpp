
#include <iostream>
#include <algorithm>

#include "constants.hpp"
#include "game.hpp"
#include "fen.hpp"
#include "arg.hpp"

/**
 * Get the options of a flag argument.
 * 
 * @param: start The start of the argument array.
 * @param: end The end of the argument array.
 * @param: option The option flag string.
 */
char* getFlagArg(char** start, char** end, std::string option) {
    char** itr = std::find(start, end, option);
    if (itr != end && itr++ != end) {
        return *itr;
    }
    return 0;
}

/**
 * Returns true if the argument is present. Else, return false.
 * 
 * @param: start The start of the argument array.
 * @param: end The end of the argument array.
 * @param: option The option flag string.
 */
bool isFlagged(char** start, char** end, std::string option) {
    if (std::find(start, end, option) != end) return true;
    return false;
}

/**
 * Parse the command line arguments.
 * 
 * @param: game The Pos struct representing the game position.
 * @param: argc The argument count.
 * @param: argv The array of command line arguments.
 */
ExitCode parseArgs(Pos* game, int argc, char *argv[], CmdLine* args) {

    // Parse FEN.
    if (isFlagged(argv, argv + argc, "-f")) {
        args->fen = getFlagArg(argv, argv + argc, "-f");
    }
    game->parseFen(args->fen);
    return NORMAL_PLY;
}

void Args::init(Pos* game, int argc, char *argv[], CmdLine* args) {
    ExitCode code = parseArgs(game, argc, argv, args);
    if (code) exit(code);
}
