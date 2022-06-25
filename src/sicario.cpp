
#include <iostream>
#include "sicario.hpp"
#include "uci.hpp"
#include "constants.hpp"

ExitCode Sicario::run() {
    std::string input("");
    do {
        std::getline(std::cin, input);
        processInput(input);
    } while (input != "quit");

    return ExitCode::NORMAL_PLY;
}

uint64_t Sicario::perft(int depth, bool print) {
    uint64_t nodes = 0;
    if (depth == 1) {
        return MoveList(position).size();
    }

    for (Move move: MoveList(position)) {
        position.makeMove(move);
        uint64_t current_node_count = perft(depth - 1);
        position.undoMove();
        nodes += current_node_count;

        if (print) {
            printMove(move, false);
            std::cout << ": " << current_node_count << '\n';
        }
    }

    return nodes;
}