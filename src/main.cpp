
#include <iostream>
#include "sicario.hpp"

void showStartUp() {
    std::cout << '\n' << NAME << " (" << VERSION << ")\n";
    std::cout << "By " << AUTHOR << '\n';
    // TODO Add cmd line option to show this info
    // std::cout << "Acknowledgments:\n";
    // std::cout << "    " << CHESS_PROGRAMMING << '\n';
    // std::cout << "    " << STOCKFISH << '\n';
    // std::cout << "    " << BLUE_FEVER_SOFT << '\n';
    // std::cout << '\n';
    std::cout << std::flush;
}

void showLogo() {
    std::cout << "\n  _______  _________  _______   ________   ________  _________  _______ \n";
    std::cout << " (  ____ \\ )__   __( (  ____ \\ (  ____  ) (  ____  ) \\__   __/ (  ___  )\n";
    std::cout << " | (    \\/    ) (    | (    \\/ | (    ) | | (    ) |    ) (    | (   ) |\n";
    std::cout << " | (_____     | |    | |       | (____) | | (____) |    | |    | |   | |\n";
    std::cout << " (_____  )    | |    | |       |  ____  | |     ___)    | |    | |   | |\n";
    std::cout << "       ) |    | |    | |       | (    ) | | (\\ (        | |    | |   | |\n";
    std::cout << " /\\____) |  __) (__  | (____/\\ | )    ( | | ) \\ \\___ ___) (___ | (___) |\n";
    std::cout << " \\_______) /_______\\ (_______/ |/      \\| |/   \\___/ )_______( (_______)\n";
    std::cout << "\n ======================== Genesis v1.0.0-alpha =========================\n\n";
    std::cout << std::flush;
}

int main(int argc, char *argv[]) {
    showLogo();
    showStartUp();
    return Sicario::run();
}
