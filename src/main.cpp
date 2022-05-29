
#include <iostream>
#include "sicario.hpp"

void showStartUp() {
    std::cout << '\n' << NAME << " (" << CODENAME << " " << VERSION << ")\n";
    std::cout << "By " << AUTHOR << "\n\n";
    // TODO Add cmd line option to show this info
    // std::cout << "Acknowledgments:\n";
    // std::cout << "    " << CHESS_PROGRAMMING << '\n';
    // std::cout << "    " << STOCKFISH << '\n';
    // std::cout << "    " << BLUE_FEVER_SOFT << '\n';
    // std::cout << '\n';
    std::cout << std::flush;
}

void showLogo() {
    std::cout << "  _______  _________  _______   ________   ________  _________  _______ \n";
    std::cout << " (  ____ \\ )__   __( (  ____ \\ (  ____  ) (  ____  ) \\__   __/ (  ___  )\n";
    std::cout << " | (    \\/    ) (    | (    \\/ | (    ) | | (    ) |    ) (    | (   ) |\n";
    std::cout << " | (_____     | |    | |       | (____) | | (____) |    | |    | |   | |\n";
    std::cout << " (_____  )    | |    | |       |  ____  | |     ___)    | |    | |   | |\n";
    std::cout << "       ) |    | |    | |       | (    ) | | (\\ (        | |    | |   | |\n";
    std::cout << " /\\____) |  __) (__  | (____/\\ | )    ( | | ) \\ \\___ ___) (___ | (___) |\n";
    std::cout << " \\_______) /_______\\ (_______/ |/      \\| |/   \\___/ )_______( (_______)\n";
}

int main(int argc, char *argv[]) {
    showLogo();
    showStartUp();
    return Sicario::run();
}
