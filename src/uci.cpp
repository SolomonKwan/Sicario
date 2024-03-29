
#include <iostream>
#include <thread>
#include <algorithm>


#include "game.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"
#include "search.hpp"

void showEogMessage(ExitCode code) {
	switch (code) {
		case NORMAL_PLY:
			std::cout << "Normal ply" << '\n';
			break;
		case WHITE_WINS:
			std::cout << "White wins" << '\n';
			break;
		case BLACK_WINS:
			std::cout << "Black wins" << '\n';
			break;
		case STALEMATE:
			std::cout << "Stalemate" << '\n';
			break;
		case THREE_FOLD_REPETITION:
			std::cout << "Three-fold repetition" << '\n';
			break;
		case FIFTY_MOVES_RULE:
			std::cout << "Fifty move rule" << '\n';
			break;
		case INSUFFICIENT_MATERIAL:
			std::cout << "Draw by insufficient material" << '\n';
			break;
	}
}

inline bool isValidTitle(std::string str) {
	return str == "GM" || str == "IM" || str == "FM" || str == "WGM" || str == "WIM" || str == "none";
}

inline bool isValidPlayerType(std::string str) {
	return str == "computer" || str == "human";
}

inline bool isValidElo(std::string str) {
	return isPostiveInteger(str) || str == "none";
}

void Sicario::processInput(const std::string& input) {
	std::vector<std::string> commands = split(input, " ");
	if (commands.size() == 0) return;
	UciInput hashedInput = hashCommandInput(commands[0]);

	switch (hashedInput) {
		case UCI:
			handleUci();
			break;
		case DEBUG:
			handleDebug(commands);
			break;
		case ISREADY:
			handleIsReady();
			break;
		case SETOPTION:
			handleSetOption(commands);
			break;
		case UCINEWGAME:
			handleUciNewGame();
			break;
		case POSITION:
			handlePosition(commands);
			break;
		case GO:
			handleGo(commands);
			break;
		case STOP:
			handleStop();
			break;
		case PONDERHIT:
			handlePonderHit();
			break;
		case QUIT:
			handleQuit();
			break;
		case INVALID_COMMAND:
			sendInvalidCommand(commands);
			break;
		case PERFT:
			handlePerft(commands);
			break;
		case MOVE:
			handleMove(commands);
			break;
		case UNDO:
			handleUndo();
			break;
		case DISPLAY:
			handleDisplay();
			break;
		case MOVES:
			handleMoves();
			break;
		case BITBOARDS:
			handleBitboards();
			break;
		case RANDOMGAME:
			handleRandom();
			break;
		case STATE:
			handleState();
			break;
		case OPTIONS:
			handleOptions();
			break;
	}
}

UciInput Sicario::hashCommandInput(const std::string& input) {
	// UCI protocol GUI to engine commands
	if (input == "uci") return UCI;
	if (input == "debug") return DEBUG;
	if (input == "isready") return ISREADY;
	if (input == "setoption") return SETOPTION;
	if (input == "ucinewgame") return UCINEWGAME;
	if (input == "position") return POSITION;
	if (input == "go") return GO;
	if (input == "stop") return STOP;
	if (input == "ponderhit") return PONDERHIT;
	if (input == "quit") return QUIT;

	// Custom command to engine
	if (input == "perft") return PERFT;
	if (input == "move") return MOVE;
	if (input == "undo") return UNDO;
	if (input == "display") return DISPLAY;
	if (input == "moves") return MOVES;
	if (input == "bitboards") return BITBOARDS;
	if (input == "random") return RANDOMGAME;
	if (input == "state") return STATE;
	if (input == "options") return OPTIONS;

	return INVALID_COMMAND;
}

ConfigOption Sicario::hashOptionsInput(const std::vector<std::string>& inputs) {
	std::string command = getOptionName(inputs);

	if (command == "thread") return THREAD;
	if (command == "hash") return HASH;
	if (command == "clear hash") return CLEAR_HASH;
	if (command == "ponder") return PONDER;
	if (command == "ownbook") return OWN_BOOK;
	if (command == "multipv") return MULTI_PV;
	if (command == "uci_showcurrLine") return UCI_SHOW_CURR_LINE;
	if (command == "uci_showrefutations") return UCI_SHOW_REFUTATIONS;
	if (command == "uci_limitstrength") return UCI_LIMIT_STRENGTH;
	if (command == "uci_elo") return UCI_ELO;
	if (command == "uci_analysemode") return UCI_ANALYSE_MODE;
	if (command == "uci_opponent") return UCI_OPPONENT;
	if (command == "expandtime") return EXPAND_TIME;

	return UNKNOWN_OPTION;
}

void Sicario::handleUci() {
	// ID information
	Uci::communicate("id name " + NAME + " (" + CODENAME + " " + VERSION + ")");
	Uci::communicate("id author " + AUTHOR + "\n");

	for (int index = THREAD; index < CONFIGS_COUNT; index++)
		Uci::sendOption(this->sicarioConfigs.options[index]);

	Uci::communicate("uciok");
}

void Sicario::handleIsReady() {
	Uci::sendReadyOk();
}

void Sicario::handleDebug(const std::vector<std::string>& inputs) {
	if (inputs.size() != 2) {
		sendMissingArgument(inputs);
	} else if (inputs[1] != "on" && inputs[1] != "off") {
		sendInvalidArgument(inputs);
	} else if (inputs.size() == 2 && inputs[1] == "on") {
		this->sicarioConfigs.debugMode = true;
	} else if (inputs.size() == 2 && inputs[1] == "off") {
		this->sicarioConfigs.debugMode = false;
	}
}

void Sicario::handleSetOption(const std::vector<std::string>& inputs) {
	ConfigOption hashedInput = hashOptionsInput(inputs);

	switch (hashedInput) {
		case THREAD:
			setOptionThread(inputs);
			break;
		case HASH:
			setOptionHash(inputs);
			break;
		case CLEAR_HASH:
			setOptionClearHash();
			break;
		case PONDER:
			setOptionPonder(inputs);
			break;
		case OWN_BOOK:
			setOptionOwnBook(inputs);
			break;
		case MULTI_PV:
			setOptionMultiPV(inputs);
			break;
		case UCI_SHOW_CURR_LINE:
			setOptionUciShowCurrLine(inputs);
			break;
		case UCI_SHOW_REFUTATIONS:
			setOptionUciShowRefutations(inputs);
			break;
		case UCI_LIMIT_STRENGTH:
			setOptionUciLimitStrength(inputs);
			break;
		case UCI_ELO:
			setOptionUciElo(inputs);
			break;
		case UCI_ANALYSE_MODE:
			setOptionUciAnalyseMode(inputs);
			break;
		case UCI_OPPONENT:
			setOptionUciOpponent(inputs);
			break;
		case EXPAND_TIME:
			setOptionExpandTime(inputs);
			break;
		case CONFIGS_COUNT:
			std::cerr << "This should not be happening..." << '\n';
			assert(false);
		case UNKNOWN_OPTION:
			sendUnknownOption(inputs);
			break;
	}
}

void Sicario::handleUciNewGame() {
	// TODO Depends on implementation. Will do at end.
}

void Sicario::handlePosition(const std::vector<std::string>& inputs) {
	this->position.resetPosition();

	if (inputs[1] == "fen") {
		// this->position.parseFen(concatFEN(inputs));
		this->position.parseFen(concatFEN(std::vector<std::string>(inputs.begin() + 2, inputs.end())));
	} else if (inputs[1] == "startpos") {
		this->position.parseFen(STANDARD_GAME);
	} else {
		sendInvalidArgument(inputs);
		return;
	}

	auto ptr = std::find(inputs.begin(), inputs.end(), "moves");
	if (ptr == inputs.end()) return;
	while (++ptr != inputs.end()) {
		this->position.processMakeMove(getMovefromAlgebraic(*ptr));
	}
}

void Sicario::handleGo(const std::vector<std::string>& commands) {
	// TODO Parse search parameters and utilise number of threads
	if (this->searchTree == false) {
		this->searchTree = true;
		this->threads.push_back(std::thread(&Sicario::search, this));
	}
}

void Sicario::handleStop() {
	this->searchTree = false;
}

void Sicario::handlePonderHit() {
	// TODO
}

void Sicario::handleQuit() {
	if (this->searchTree == true) this->searchTree = false;
	for (std::thread& thr : this->threads) {
		if (thr.joinable()) thr.join();
	}
}

void Sicario::handlePerft(const std::vector<std::string>& commands) {
	auto start = std::chrono::high_resolution_clock::now();
	uint64_t totalNodes = perft(std::stoi(commands[1]), true);
	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Nodes searched: " << totalNodes << '\n';

	if (this->sicarioConfigs.debugMode) {
		std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "μs\n";
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms\n";
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() << "s\n";
	}
}

void Sicario::handleMove(const std::vector<std::string>& commands) {
	Move move = getMovefromAlgebraic(commands[1]);
	if (move == NULL_MOVE) {
		Uci::communicate("Invalid move: " + move);
		return;
	}

	MoveList moves = MoveList(this->position);
	if (!moves.contains(move) && !moves.contains(move | EN_PASSANT)) { // NOTE en-passant checks are hacky...
		Uci::communicate("Invalid move");
		return;
	}

	if (moves.contains(move)) {
		this->position.processMakeMove(move);
	} else {
		this->position.processMakeMove(move | EN_PASSANT);
	}
}

void Sicario::handleUndo() {
	this->position.processUndoMove();
}

void Sicario::handleDisplay() {
	this->position.display();
}

void Sicario::handleMoves() {
	MoveList moves = MoveList(this->position);
	std::cout << "There are " << moves.size() << " moves" << '\n';
	for (Move move : moves) {
		printMove(move, true);
		std::cout << '\n';
	}
}

void Sicario::handleBitboards() {
	this->position.displayBitboards();
}

void Sicario::handleRandom() {
	for (int i = 0; i < 1000000000; i++) {
		MoveList moves = MoveList(this->position);
		std::vector<Move> playedMoves;
		int moveCount = 0;
		while (!this->position.isEOG(moves)) {
			Move move = moves.randomMove();
			playedMoves.push_back(move);
			this->position.processMakeMove(move);
			moves = MoveList(this->position);
			moveCount++;
		}
		showEogMessage(this->position.isEOG(moves));
		while (moveCount > 0) {
			this->position.processUndoMove();
			moveCount--;
		}
	}
}

void Sicario::handleState() {
	MoveList moves = MoveList(this->position);
	showEogMessage(this->position.isEOG(moves));
}

void Sicario::handleOptions() {
	for (int index = THREAD; index < CONFIGS_COUNT; index++) {
		std::cout << sicarioConfigs.options[index].name << ' ';
		std::cout << sicarioConfigs.options[index].value << '\n';
	}
}

void Uci::communicate(std::string communication) {
	std::cout << communication << '\n';
}

void Uci::sendReadyOk() {
	Uci::communicate("readyok");
}

void Uci::sendBestMove(MctsNode* root, bool debugMode) {
	std::cout << "bestmove ";
	printMove(root->bestChild()->getInEdge(), false, true);

	if (!debugMode) return;

	std::vector<float> ucb1Values;
	for (auto child : root->getChildren())
		ucb1Values.push_back(child->Ucb1());
	std::vector<size_t> ucbRanks = rankSort(ucb1Values);

	for (auto child : root->getChildren()) {
		printMove(child->getInEdge(), true);
		std::cout << "\tValue: " << child->getValue();
		std::cout << "\tVisits: " << child->getVisits();
		std::cout << "\tUCB1: " << ucb1Values.front();
		std::cout << "\tRank: " << ucbRanks.front() << '\n';
		ucb1Values.erase(ucb1Values.begin());
		ucbRanks.erase(ucbRanks.begin());
	}
}

void Uci::sendCopyProtection() {

}

void Uci::sendRegistration() {

}

void Uci::sendInfo(SearchInfo& searchInfo) {
	std::cout << "depth " << searchInfo.depth << '\n';
}

void Uci::sendOption(const OptionInfo& option) {
	std::string optionString = "option name " + option.name + " type " + option.type + " default " + option.def;
	if (option.min != "") optionString += " min " + option.min + " max " + option.max;
	for (std::string var : option.vars) optionString += " var " + var;
	Uci::communicate(optionString);
}

void Sicario::sendInvalidCommand(const std::vector<std::string>& commands) {
	Uci::communicate("Unknown command: " + commands[0]);
}

void Sicario::sendMissingArgument(const std::vector<std::string>& inputs) {
	Uci::communicate("Missing argument: " + concat(inputs, " "));
}

void Sicario::sendInvalidArgument(const std::vector<std::string>& inputs) {
	Uci::communicate("Invalid argument: " + concat(inputs, " "));
}

void Sicario::sendUnknownOption(const std::vector<std::string>& inputs) {
	Uci::communicate("Unknown option: " + getOptionName(inputs));
}

void Sicario::sendInvalidValue(const std::vector<std::string>& inputs) {
	Uci::communicate("Invalid argument: " + getOptionValue(inputs));
}

void Sicario::sendArgumentOutOfRange(const std::vector<std::string>& inputs) {
	Uci::communicate("Argument out of range: " + getOptionValue(inputs));
}

void Sicario::setOptionThread(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[THREAD];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	option.value = value;
}

void Sicario::setOptionHash(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[HASH];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	option.value = value;
}

void Sicario::setOptionClearHash() {
	// TODO
}

void Sicario::setOptionPonder(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[PONDER];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionOwnBook(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[OWN_BOOK];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionMultiPV(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[MULTI_PV];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	option.value = value;
}

void Sicario::setOptionUciShowCurrLine(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[UCI_SHOW_CURR_LINE];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciShowRefutations(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[UCI_SHOW_REFUTATIONS];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciLimitStrength(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[UCI_LIMIT_STRENGTH];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciElo(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[UCI_ELO];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	option.value = value;
}

void Sicario::setOptionUciAnalyseMode(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[UCI_ANALYSE_MODE];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciOpponent(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	std::vector<std::string> values = split(value, " ");
	OptionInfo& option = sicarioConfigs.options[UCI_OPPONENT];
	if (values.size() < 4 || !isValidTitle(values[0]) || !isValidElo(values[1]) || !isValidPlayerType(values[2])) {
		sendInvalidArgument(inputs);
		return;
	}
	option.value = value;
}

void Sicario::setOptionExpandTime(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	OptionInfo& option = sicarioConfigs.options[EXPAND_TIME];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	option.value = value;
}

std::string Sicario::getOptionName(const std::vector<std::string>& inputs) {
	auto nameItr = std::find(inputs.begin(), inputs.end(), "name");
	auto valueItr = std::find(inputs.begin(), inputs.end(), "value");
	unsigned long nameIndex = nameItr - inputs.begin();

	if (nameItr == inputs.end() || nameIndex != 1 || nameIndex + 1 > inputs.size() - 1) return "";
	std::string name = concat(std::vector<std::string>(nameItr + 1, valueItr), " ");
	std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
	return name;
}

std::string Sicario::getOptionValue(const std::vector<std::string>& inputs) {
	auto nameItr = std::find(inputs.begin(), inputs.end(), "name");
	auto valueItr = std::find(inputs.begin(), inputs.end(), "value");
	unsigned long valueIndex = valueItr - inputs.begin();

	if (nameItr == inputs.end() || valueItr == inputs.end() || valueIndex + 1 > inputs.size() - 1) return "";
	return concat(std::vector<std::string>(valueItr + 1, inputs.end()), " ");
}