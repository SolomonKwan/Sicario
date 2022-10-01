
#include <iostream>
#include "sicario.hpp"
#include "uci.hpp"
#include "constants.hpp"
#include "utils.hpp"

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

Position Sicario::getPosition() const {
	return position;
}

ExitCode Sicario::run() {
	std::string input("");
	do {
		std::getline(std::cin, input);
		processInput(input);
	} while (input != "quit");

	searchTree = false;
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
			position.processMakeMove(move, false);
			current_node_count = depth == 2 ? MoveList(position).size() : perft(depth - 1);
			position.processUndoMove();
			nodes += current_node_count;
		}

		if (root) {
			printMove(move, false);
			std::cout << ": " << current_node_count << '\n';
		}
	}

	return nodes;
}