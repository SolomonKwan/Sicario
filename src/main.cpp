
#include <iostream>
#include "movegen.hpp"
#include "arg.hpp"
#include "uci.hpp"
#include "game.hpp"

namespace Run {
    void init(Game* game, Computed* moves, CmdLine* args, char *argv[]) {
        std::string input;
        std::getline(std::cin, input);
        if (input == "uci") {
            UCI::init(game, moves, args, argv, input);
        } else {
            Play::init(game, moves, args, argv, input);
        }
    }
}

int main(int argc, char *argv[]) {
    CmdLine args;
    Computed moves;
    Game game;

    // Initialisation.
    Compute::init(&moves);
    Args::init(&game, argc, argv, &args);
    Run::init(&game, &moves, &args, argv);

    return 0;
}
