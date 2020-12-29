
#include <iostream>
#include "movegen.hpp"
#include "uci.hpp"
#include "game.hpp"

namespace Run {
    #define VERSION "(Alpha v1.0.0)"
    #define AUTHOR "S. Kwan"
    #define CHESS_PROGRAMMING "www.chessprogramming.org"
    #define STOCKFISH "Stockfish"
    #define BLUE_FEVER_SOFT "bluefeversoft"

    void showStartUp() {
        std::cout << "\nSicario " << VERSION << '\n';
        std::cout << "By " << AUTHOR << '\n';
        std::cout << "Acknowledgments:\n";
        std::cout << "    " << CHESS_PROGRAMMING << '\n';
        std::cout << "    " << STOCKFISH << '\n';
        std::cout << "    " << BLUE_FEVER_SOFT << '\n';
        std::cout << '\n';
    }

    void showLogo() {
        std::cout << "\n   _______ _________ _______  _______  _______ _________ _______ \n";
        std::cout << "  (  ____ \\\\__   __/(  ____ \\(  ___  )(  ____ )\\__   __/(  ___  )\n";
        std::cout << "  | (    \\/   ) (   | (    \\/| (   ) || (    )|   ) (   | (   ) |\n";
        std::cout << "  | (_____    | |   | |      | (___) || (____)|   | |   | |   | |\n";
        std::cout << "  (_____  )   | |   | |      |  ___  ||     __)   | |   | |   | |\n";
        std::cout << "        ) |   | |   | |      | (   ) || (\\ (      | |   | |   | |\n";
        std::cout << "  /\\____) |___) (___| (____/\\| )   ( || ) \\ \\_____) (___| (___) |\n";
        std::cout << "  \\_______)\\_______/(_______/|/     \\||/   \\__/\\_______/(_______)\n\n";
    }

    void init() {
        showLogo();
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
