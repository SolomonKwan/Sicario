
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
ExitCode parseArgs(Pos* game, int argc, char *argv[], 
        CmdLine* args) {

    // Version
    if (isFlagged(argv, argv + argc, "--version") || isFlagged(argv, argv + argc, "-v")) {
        std::cout << '\n' << VERSION << '\n';
        std::cout << "By " << AUTHOR << '\n';
        std::cout << "Acknowledgments:\n";
        std::cout << "    " << CHESS_PROGRAMMING << '\n';
        std::cout << "    " << STOCKFISH << '\n';
        std::cout << "    " << BLUE_FEVER_SOFT << '\n';
        std::cout << '\n';
        exit(0);
    }

    // Help.
    if (isFlagged(argv, argv + argc, "-h") || isFlagged(argv, argv + argc, 
            "--help")) {
        showUsage(argv);
        exit(0);
    }

    // Check arg number.
    if (argc < 3) {
        game->showEOG(INVALID_ARGS, argv);
        return INVALID_ARGS;
    }
    
    // Process types of players (human or computer).
    std::string h = "h", c = "c";
    if ((argv[1] != h && argv[1] != c) || (argv[2] != h && argv[2] != c)) {
        game->showEOG(INVALID_ARGS, argv);
        return INVALID_ARGS;
    }
    args->white = argv[1] == h ? HUMAN : COMPUTER;
    args->black = argv[2] == h ? HUMAN : COMPUTER;

    // Parse FEN.
    if (isFlagged(argv, argv + argc, "-f")) {
        args->fen = getFlagArg(argv, argv + argc, "-f");
    }
    game->parseFen(args->fen);

    // Get dark mode
    args->dark_mode = isFlagged(argv, argv + argc, "-i");

    // Get quiet mode
    args->quiet = isFlagged(argv, argv + argc, "-q");

    return NORMAL_PLY;
}

void Args::init(Pos* game, int argc, char *argv[], CmdLine* args) {
    ExitCode code = parseArgs(game, argc, argv, args);
    if (code) exit(code);
}
