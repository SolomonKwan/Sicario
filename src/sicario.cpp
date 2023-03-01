
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

Sicario::Sicario() {
	// NOTE Currently, the default, min, max and var are all arbitrary
	// Set the config informations
	sicarioConfigs.options[THREAD] = { "Thread", "spin", "1", "1", "512" };
	sicarioConfigs.options[HASH] = { "Hash", "spin", "16", "0", "5000" };
	sicarioConfigs.options[PONDER] = { "Ponder", "check", "false" };
	sicarioConfigs.options[OWN_BOOK] = { "OwnBook", "check", "false" };
	sicarioConfigs.options[MULTI_PV] = { "MultiPV", "spin", "1", "1", "5" };
	sicarioConfigs.options[UCI_SHOW_CURR_LINE] = { "UCI_ShowCurrLine", "check", "false" };
	sicarioConfigs.options[UCI_SHOW_REFUTATIONS] = { "UCI_ShowRefutations", "check", "false" };
	sicarioConfigs.options[UCI_LIMIT_STRENGTH] = { "UCI_LimitStrength", "check", "false" };
	sicarioConfigs.options[UCI_ELO] = { "UCI_Elo", "spin", "3000", "1000", "3500" };
	sicarioConfigs.options[UCI_ANALYSE_MODE] = { "UCI_AnalyseMode", "check", "true" };
	sicarioConfigs.options[UCI_OPPONENT] = { "UCI_Opponent", "string", "" };
	sicarioConfigs.options[EXPAND_TIME] = { "ExpandTime", "spin", "500", "100", "5000" };
	sicarioConfigs.options[CLEAR_HASH] = { "ClearHash", "button" };

	// Set the configs to the default
	for (int index = THREAD; index < CONFIGS_COUNT; index++)
		sicarioConfigs.options[index].value = sicarioConfigs.options[index].def;

	#ifndef NDEBUG
	for (int index = THREAD; index < CONFIGS_COUNT; index++)
		assert(sicarioConfigs.options[index].name != "");
	#endif
}

void Sicario::run() {
	std::string input("");
	do {
		std::getline(std::cin, input);
		processInput(input);
	} while (input != "quit");

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
			this->position.processMakeMove(move, false);
			current_node_count = depth == 2 ? MoveList(this->position).size() : perft(depth - 1);
			this->position.processUndoMove();
			nodes += current_node_count;
		}

		if (root) {
			printMove(move, false);
			std::cout << ": " << current_node_count << '\n';
		}
	}

	return nodes;
}