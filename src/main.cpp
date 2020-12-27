
#include <iostream>
#include "movegen.hpp"
#include "arg.hpp"
#include "uci.hpp"
#include "game.hpp"

namespace Run {
    void init(Pos* game, CmdLine* args, char *argv[]) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "uci") {
            // UCI::init(game, moves, args, argv, input);
        } else {
            Play::init(game, args, argv, input);
        }
    }
}

int main(int argc, char *argv[]) {
    CmdLine args;
    Pos game;

    // Initialisation.
    Args::init(&game, argc, argv, &args);
    Run::init(&game, &args, argv);

    return 0;
}
