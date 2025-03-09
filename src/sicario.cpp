
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
		std::cout << "Refer to README for more information." << '\n';
		exit(0);
	}
}

void showLogo() {
	std::cout << "  _______  _________  _______   ________   ________  _________  _______\n";
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

template<>
std::string OptionConfig<int>::toUciString() const {
	std::string optionString = "option name " + this->name + " type " + this->type;
	optionString += " default " + std::to_string(this->def);
	optionString += " min " + std::to_string(this->min);
	optionString += " max " + std::to_string(this->max);
	return optionString;
}

template<>
std::string OptionConfig<bool>::toUciString() const {
	std::string optionString = "option name " + this->name + " type " + this->type;
	optionString += (this->type == "check" ? " default " + std::string(this->def ? "true" : "false") : "");
	return optionString;
}

template<>
std::string OptionConfig<std::string>::toUciString() const {
	std::string optionString = "option name " + this->name + " type " + this->type;
	optionString += " default " + this->def;
	optionString += (this->vars.size() ? " var " + concat(this->vars, " var ") : "");
	return optionString;
}

template<>
std::string OptionConfig<int>::toString() const {
	return this->name + " " + std::to_string(this->value);
}

template<>
std::string OptionConfig<bool>::toString() const {
	return this->name + " " + (this->value ? "true" : "false");
}

template<>
std::string OptionConfig<std::string>::toString() const {
	return this->name + " " + this->value;
}

void Sicario::run() {
	std::string input("");
	while (input != "quit") {
		std::getline(std::cin, input);
		processInput(input);
	}
	this->searchTree = false;
}

uint64_t Sicario::perft(const int depth, const bool root) {
	uint64_t nodes = 0;
	uint64_t current_node_count = 0;

	for (Move move: MoveList(this->position)) {
		if (depth <= 1) {
			current_node_count = 1;
			nodes++;
		} else {
			this->position.makeMove(move, false);
			current_node_count = depth == 2 ? MoveList(this->position).size() : perft(depth - 1);
			this->position.undoMove();
			nodes += current_node_count;
		}

		if (root)
			std::cout << getMove(move) << ": " << current_node_count << '\n';
	}

	return nodes;
}

void SearchParams::reset() {
	this->searchMoves.clear();
	this->ponder = false;
	this->wtime = -1;
	this->btime = -1;
	this->winc = -1;
	this->binc = -1;
	this->movesToGo = -1;
	this->depth = -1;
	this->nodes = -1;
	this->mate = -1;
	this->moveTime = -1;
	this->infinite = false;
	this->suddenDeath = false;
}