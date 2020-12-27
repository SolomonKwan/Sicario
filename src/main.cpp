
#include <iostream>
#include "movegen.hpp"
#include "uci.hpp"
#include "game.hpp"

namespace Run {
    #define VERSION "alpha v1.0.0"
    #define AUTHOR "S. Kwan"
    #define CHESS_PROGRAMMING "www.chessprogramming.org"
    #define STOCKFISH "Stockfish"
    #define BLUE_FEVER_SOFT "bluefeversoft"

    void showStartUp() {
        std::cout << "\nnameless (to be named) " << VERSION << '\n';
        std::cout << "By " << AUTHOR << '\n';
        std::cout << "Acknowledgments:\n";
        std::cout << "    " << CHESS_PROGRAMMING << '\n';
        std::cout << "    " << STOCKFISH << '\n';
        std::cout << "    " << BLUE_FEVER_SOFT << '\n';
        std::cout << '\n';
    }

    void init() {
        showStartUp();
        std::string input;
        std::getline(std::cin, input);
        if (input == "uci") {
            // UCI::init(input);
        } else {
            Play::init(input);
        }
    }
}

int main(int argc, char *argv[]) {
    Run::init();
    return 0;
}
