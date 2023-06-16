
#include <iostream>
#include <thread>
#include <algorithm>
#include <ranges>

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
			handleRandom(commands);
			break;
		case STATE:
			handleState();
			break;
		case OPTIONS:
			handleOptions();
			break;
		case DATA:
			handleData();
			break;
		case INVALID_COMMAND:
		default:
			sendInvalidCommand(commands);
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
	if (input == "history") return DATA;

	return INVALID_COMMAND;
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
		sendInvalidArgument(inputs[1]);
	} else if (inputs.size() == 2 && inputs[1] == "on") {
		this->sicarioConfigs.debugMode = true;
	} else if (inputs.size() == 2 && inputs[1] == "off") {
		this->sicarioConfigs.debugMode = false;
	}
}

void Sicario::handleSetOption(const std::vector<std::string>& inputs) {
	const auto[ name, value ] = getOptionNameAndValue(inputs);

	if (name == "thread") return setOptionThread(value);
	else if (name == "hash") return setOptionHash(value);
	else if (name == "ponder") return setOptionPonder(value);
	else if (name == "ownbook") return setOptionOwnBook(value);
	else if (name == "multipv") return setOptionMultiPV(value);
	else if (name == "uci_showcurrline") return setOptionUciShowCurrLine(value);
	else if (name == "uci_showrefutations") return setOptionUciShowRefutations(value);
	else if (name == "uci_limitstrength") return setOptionUciLimitStrength(value);
	else if (name == "uci_elo") return setOptionUciElo(value);
	else if (name == "uci_analysemode") return setOptionUciAnalyseMode(value);
	else if (name == "uci_opponent") return setOptionUciOpponent(value);
	else if (name == "expandtime") return setOptionExpandTime(value);
	else if (name == "clearhash") return setOptionClearHash();
	else {
		std::cerr << "This should not be happening..." << '\n';
		sendUnknownOption(name);
		assert(false);
	}
}

void Sicario::handleUciNewGame() {
	this->getPosition().clearData();
}

void Sicario::handlePosition(const std::vector<std::string>& inputs) {
	this->position.resetPosition();

	if (inputs[1] == "fen") {
		this->position.parseFen(concatFEN(std::vector<std::string>(inputs.begin() + 2, inputs.end())));
	} else if (inputs[1] == "startpos") {
		this->position.parseFen(STANDARD_GAME);
	} else {
		sendInvalidArgument(inputs[1]);
		return;
	}

	auto ptr = std::find(inputs.begin(), inputs.end(), "moves");
	if (ptr == inputs.end()) return;
	while (++ptr != inputs.end()) {
		this->position.processMakeMove(getMovefromAlgebraic(*ptr));
	}
}

void Sicario::handleGo(const std::vector<std::string>& inputs) {
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

void Sicario::handlePerft(const std::vector<std::string>& inputs) {
	auto start = std::chrono::high_resolution_clock::now();
	uint64_t totalNodes = perft(std::stoi(inputs[1]), true);
	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Nodes searched: " << totalNodes << '\n';

	if (this->sicarioConfigs.debugMode) {
		std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "μs\n";
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms\n";
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() << "s\n";
	}
}

void Sicario::handleMove(const std::vector<std::string>& inputs) {
	Move move = getMovefromAlgebraic(inputs[1]);
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

void Sicario::handleRandom(const std::vector<std::string> &inputs) {
	std::unordered_map<ExitCode, int> results;
	int iterations = inputs.size() == 2 ? std::stoi(inputs[1]) : 100;
	for (int i = 0; i < iterations; i++) {
		MoveList moves = MoveList(this->position);
		int moveCount = 0;
		while (!this->position.isEOG(moves)) {
			Move move = moves.randomMove();
			this->position.processMakeMove(move);
			moves = MoveList(this->position);
			moveCount++;
		}
		results[this->position.isEOG(moves)]++;

		// Undo moves back to original position.
		while (moveCount > 0) {
			this->position.processUndoMove();
			moveCount--;
		}
	}

	// Print overall results
	int total = 0;
	for (auto pair : results) total += pair.second;
	printf("WHITE_WINS: \t\t%d\t %.2f%%\n", results[WHITE_WINS],
			static_cast<float>(results[WHITE_WINS]) / total * 100);
	printf("BLACK_WINS: \t\t%d\t %.2f%%\n", results[BLACK_WINS],
			static_cast<float>(results[BLACK_WINS]) / total * 100);
	printf("STALEMATE: \t\t%d\t %.2f%%\n", results[STALEMATE],
			static_cast<float>(results[STALEMATE]) / total * 100);
	printf("THREE_FOLD_REPETITION: \t%d\t %.2f%%\n", results[THREE_FOLD_REPETITION],
			static_cast<float>(results[THREE_FOLD_REPETITION]) / total * 100);
	printf("FIFTY_MOVES_RULE: \t%d\t %.2f%%\n", results[FIFTY_MOVES_RULE],
			static_cast<float>(results[FIFTY_MOVES_RULE]) / total * 100);
	printf("INSUFFICIENT_MATERIAL: \t%d\t %.2f%%\n", results[INSUFFICIENT_MATERIAL],
			static_cast<float>(results[INSUFFICIENT_MATERIAL]) / total * 100);
}

void Sicario::handleState() {
	MoveList moves = MoveList(this->position);
	showEogMessage(this->position.isEOG(moves));
}

void Sicario::handleOptions() {
	for (int index = THREAD; index < CONFIGS_COUNT; index++) {
		std::cout << sicarioConfigs.options[index].name;
		if (sicarioConfigs.options[index].value != "")
			std::cout << ' ' << sicarioConfigs.options[index].value << '\n';
		else
			std::cout << '\n';
	}
}

void Sicario::handleData() {
	// Print the move history.
	const std::vector<History>& historyVec = this->getPosition().getHistory();
	for (const History& history : historyVec) {
		printMove(history.move, false);
		if (&history != &historyVec.back()) std::cout << ' ';
	}
	std::cout << '\n';

	// Print the positionCounts size.
	std::cout << this->getPosition().getPositionCounts().size() << '\n';
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
	std::string optionString =
			"option name " + option.name + " type " + option.type + (option.def != "" ? " default " + option.def : "");
	if (option.min != "") optionString += " min " + option.min + " max " + option.max;
	for (std::string var : option.vars) optionString += " var " + var;
	Uci::communicate(optionString);
}

void Sicario::sendInvalidCommand(const std::vector<std::string>& inputs, const std::string& customMsg) {
	Uci::communicate("Unknown command: " + (customMsg == "" ? inputs[0] : customMsg));
}

void Sicario::sendMissingArgument(const std::vector<std::string>& inputs, const std::string& customMsg) {
	Uci::communicate("Missing argument: " + (customMsg == "" ? concat(inputs, " ") : customMsg));
}

void Sicario::sendInvalidArgument(const std::string& value, const std::string& customMsg) {
	Uci::communicate("Invalid argument: " + (customMsg == "" ? value : customMsg));
}

void Sicario::sendUnknownOption(const std::string& option, const std::string& customMsg) {
	Uci::communicate("Unknown option: " + (customMsg == "" ? option : customMsg));
}

void Sicario::sendInvalidValue(const std::string& value, const std::string& customMsg) {
	Uci::communicate("Invalid argument: " + (customMsg == "" ? value : customMsg));
}

void Sicario::sendArgumentOutOfRange(const std::string& value, const std::string& customMsg) {
	Uci::communicate("Argument out of range: " + (customMsg == "" ? value : customMsg));
}

void Sicario::setOptionThread(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[THREAD];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(value);
		return;
	}
	option.value = value;
}

void Sicario::setOptionHash(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[HASH];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(value);
		return;
	}
	option.value = value;
}

void Sicario::setOptionPonder(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[PONDER];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setOptionOwnBook(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[OWN_BOOK];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setOptionMultiPV(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[MULTI_PV];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(value);
		return;
	}
	option.value = value;
}

void Sicario::setOptionUciShowCurrLine(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[UCI_SHOW_CURR_LINE];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setOptionUciShowRefutations(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[UCI_SHOW_REFUTATIONS];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setOptionUciLimitStrength(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[UCI_LIMIT_STRENGTH];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setOptionUciElo(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[UCI_ELO];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(value);
		return;
	}
	option.value = value;
}

void Sicario::setOptionUciAnalyseMode(const std::string& value) {
	if (value == "") return;
	OptionInfo& option = sicarioConfigs.options[UCI_ANALYSE_MODE];
	if (value == "true" || value == "false") {
		option.value = value;
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setOptionUciOpponent(const std::string& value) {
	std::vector<std::string> values = split(value, " ");
	OptionInfo& option = sicarioConfigs.options[UCI_OPPONENT];
	if (value != "" && (values.size() < 4 || !isValidTitle(values[0]) || !isValidElo(values[1]) ||
			!isValidPlayerType(values[2]))) {
		sendInvalidArgument(value, "[GM|IM|FM|WGM|WIM|none] [<elo>|none] [computer|human] <name>");
		return;
	}
	option.value = value;
}

void Sicario::setOptionExpandTime(const std::string& value) {
	OptionInfo& option = sicarioConfigs.options[EXPAND_TIME];
	if (std::stoi(value) < std::stoi(option.min) || std::stoi(value) > std::stoi(option.max)) {
		sendArgumentOutOfRange(value);
		return;
	}
	option.value = value;
}

void Sicario::setOptionClearHash() {
	// TODO
}

SetOptionPair Sicario::getOptionNameAndValue(const std::vector<std::string>& inputs) {
	std::string name = "";
	std::string value = "";
	bool buildValue = false;
	const int SKIP_SETOPTION_NAME_STRING = 2;

	// Extract name and value strings.
	for (std::string str : inputs | std::views::drop(SKIP_SETOPTION_NAME_STRING)) {
		if (str == "value") {
			buildValue = true;
			continue;
		}
		if (!buildValue) name += str + ' ';
		else value += str + ' ';
	}

	// Convert name to all lower case.
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	return { trim(name), trim(value) };
}