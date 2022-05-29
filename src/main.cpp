
#include <iostream>
#include "sicario.hpp"

// TODO Add custom commands explanations to readme
void showInfo(int argc, char* argv[]) {
    if (argc < 2) return;
    if (argv[1] == std::string("-h") || argv[1] == std::string("--help")) {
        std::cout << NAME << " (" << CODENAME << " " << VERSION << ")\n";
        std::cout << "By " << AUTHOR << '\n';
        std::cout << "Refer to README for more information on how to use." << '\n';
        std::cout << "Acknowledgments:\n";
        std::cout << "    " << CHESS_PROGRAMMING << '\n';
        std::cout << "    " << STOCKFISH << '\n';
        std::cout << "    " << BLUE_FEVER_SOFT << '\n';
        exit(0);
    }
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

void showStartUp() {
    std::cout << '\n' << NAME << " (" << CODENAME << " " << VERSION << ")\n";
    std::cout << "By " << AUTHOR << "\n\n";
}

int main(int argc, char* argv[]) {
    showInfo(argc, argv);
    showLogo();
    showStartUp();
    return Sicario::run();
}
