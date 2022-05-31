
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
