
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

uint64_t Sicario::perft(int depth, bool root) {
    uint64_t nodes = 0;
    uint64_t current_node_count = 0;

    for (Move move: MoveList(position)) {
        if (depth <= 1) {
            current_node_count = 1;
            nodes++;
        } else {
            position.makeMove(move);
            current_node_count = depth == 2 ? MoveList(position).size() : perft(depth - 1);
            position.undoMove();
            nodes += current_node_count;
        }

        if (root) {
            printMove(move, false);
            std::cout << ": " << current_node_count << '\n';
        }
    }

    return nodes;
}