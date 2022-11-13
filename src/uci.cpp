
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

	return UNKNOWN_OPTION;
}

void Sicario::handleUci() {
	// ID information
	communicate("id name " + NAME + " (" + CODENAME + " " + VERSION + ")");
	communicate("id author " + AUTHOR + "\n");

	// Configurable options (see SicarioConfigs struct)
	sendOption(this->thread);
	sendOption(this->hash);
	sendOption(this->ponder);
	sendOption(this->ownBook);
	sendOption(this->multiPv);
	sendOption(this->uciShowCurrLine);
	sendOption(this->uciShowRefutations);
	sendOption(this->uciLimitStrength);
	sendOption(this->uciElo);
	sendOption(this->uciAnalyseMode);
	sendOption(this->uciOpponent);

	communicate("uciok");
}

void Sicario::handleIsReady() {
	sendReadyOk();
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
		case UNKNOWN_OPTION:
			sendUnknownOption(inputs);
			break;
	}
}

void Sicario::handleUciNewGame() {
	// TODO Depends on implementation. Will do at end.
}

void Sicario::handlePosition(const std::vector<std::string>& inputs) {
	// TODO play out moves as well and handle startpos command and unknown commands
	if (inputs[1] == "fen") {
		this->position.parseFen(concatFEN(inputs));
	}
}

void Sicario::handleGo(const std::vector<std::string>& commands) {
	// TODO Parse search parameters
	if (this->searchTree == false) {
		this->searchTree = true;
		std::thread searchThread(&Sicario::mcts, this);
		searchThread.detach();
	}
}

void Sicario::handleStop() {
	this->searchTree = false;
}

void Sicario::handlePonderHit() {
	// TODO
}

void Sicario::handlePerft(const std::vector<std::string>& commands) {
	uint64_t totalNodes = perft(std::stoi(commands[1]), true);
	std::cout << "Nodes searched: " << totalNodes << '\n';
}

void Sicario::handleMove(const std::vector<std::string>& commands) {
	// TODO chuck this in a parse move
	std::string move_str = commands[1];
	Move move = 0;
	if (move_str[0] < 'a' || move_str[0] > 'h' || move_str[2] < 'a' || move_str[2] > 'h') {
		communicate("Invalid move");
		return;
	}

	if (move_str[1] < '1' || move_str[1] > '8' || move_str[3] < '1' || move_str[3] > '8') {
		communicate("Invalid move");
		return;
	}

	int start_file = move_str[0] - 'a';
	int start_rank = move_str[1] - '1';
	int end_file = move_str[2] - 'a';
	int end_rank = move_str[3] - '1';

	if (move_str.length() == 5) {
		move |= PROMOTION;
		if (move_str[4] == 'q') {
			move |= pQUEEN;
		} else if (move_str[4] == 'r') {
			move |= pROOK;
		} else if (move_str[4] == 'b') {
			move |= pBISHOP;
		}
	}

	move |= 8 * start_rank + start_file;
	move |= ((8 * end_rank + end_file) << 6);

	MoveList moves = MoveList(this->position);
	if (!moves.contains(move) && !moves.contains(move | EN_PASSANT)) { // NOTE en-passant checks are hacky...
		communicate("Invalid move");
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

	std::cout << moves.moves_index << '\n';
	for (uint i = 0; i < moves.moves_index; i++) {
		std::cout << moves.moveSets[i]->size() << '\n';
	}

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
	std::cout << "debugMode " << this->sicarioConfigs.debugMode << '\n';
	std::cout << "threads " << this->sicarioConfigs.threads << '\n';
	std::cout << "hash " << this->sicarioConfigs.hash << '\n';
	std::cout << "ponder " << this->sicarioConfigs.ponder << '\n';
	std::cout << "ownBook " << this->sicarioConfigs.ownBook << '\n';
	std::cout << "multiPv " << this->sicarioConfigs.multiPv << '\n';
	std::cout << "uciShowCurrLine " << this->sicarioConfigs.uciShowCurrLine << '\n';
	std::cout << "uciShowRefutations " << this->sicarioConfigs.uciShowRefutations << '\n';
	std::cout << "uciLimitStrength " << this->sicarioConfigs.uciLimitStrength << '\n';
	std::cout << "uciElo " << this->sicarioConfigs.uciElo << '\n';
	std::cout << "uciAnalyseMode " << this->sicarioConfigs.uciAnalyseMode << '\n';
	std::cout << "uciOpponent " << this->sicarioConfigs.uciOpponent << '\n';
}

void Sicario::communicate(std::string communication) {
	std::cout << communication << '\n';
}

void Sicario::sendOption(const OptionInfo& option) {
	std::string optionString = "option name " + option.name + " type " + option.type + " default " + option.def;
	if (option.min != "") optionString += " min " + option.min + " max " + option.max;
	for (std::string var : option.vars) optionString += " var " + var;
	communicate(optionString);
}

/**
 * Sends the is readyok.
 */
void Sicario::sendReadyOk() {
	communicate("readyok");
}

void Sicario::sendBestMove() {

}

void Sicario::sendCopyProtection() {

}

void Sicario::sendRegistration() {

}

void Sicario::sendInfo() {

}

void Sicario::sendInvalidCommand(const std::vector<std::string>& commands) {
	communicate("Unknown command: " + commands[0]);
}

void Sicario::sendMissingArgument(const std::vector<std::string>& inputs) {
	communicate("Missing argument: " + concat(inputs, " "));
}

void Sicario::sendInvalidArgument(const std::vector<std::string>& inputs) {
	communicate("Invalid argument: " + concat(inputs, " "));
}

void Sicario::sendUnknownOption(const std::vector<std::string>& inputs) {
	communicate("Unknown option: " + getOptionName(inputs));
}

void Sicario::sendInvalidValue(const std::vector<std::string>& inputs) {
	communicate("Invalid argument: " + getOptionValue(inputs));
}

void Sicario::sendArgumentOutOfRange(const std::vector<std::string>& inputs) {
	communicate("Argument out of range: " + getOptionValue(inputs));
}

void Sicario::setOptionThread(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (!isPostiveInteger(value)) {
		sendInvalidArgument(inputs);
		return;
	}

	if (std::stoi(value) < std::stoi(this->thread.min) || std::stoi(value) > std::stoi(this->thread.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	this->sicarioConfigs.hash = std::stoi(value);
}

void Sicario::setOptionHash(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (!isPostiveInteger(value)) {
		sendInvalidArgument(inputs);
		return;
	}

	if (std::stoi(value) < std::stoi(this->hash.min) || std::stoi(value) > std::stoi(this->hash.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	this->sicarioConfigs.hash = std::stoi(value);
}

void Sicario::setOptionClearHash() {
	// TODO
}

void Sicario::setOptionPonder(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (value == "true") {
		this->sicarioConfigs.ponder = true;
	} else if (value == "false") {
		this->sicarioConfigs.ponder = false;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionOwnBook(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (value == "true") {
		this->sicarioConfigs.ownBook = true;
	} else if (value == "false") {
		this->sicarioConfigs.ownBook = false;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionMultiPV(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (!isPostiveInteger(value)) {
		sendInvalidArgument(inputs);
		return;
	}

	if (std::stoi(value) < std::stoi(this->multiPv.min) || std::stoi(value) > std::stoi(this->multiPv.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	this->sicarioConfigs.multiPv = std::stoi(value);
}

void Sicario::setOptionUciShowCurrLine(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (value == "true") {
		this->sicarioConfigs.uciShowCurrLine = true;
	} else if (value == "false") {
		this->sicarioConfigs.uciShowCurrLine = false;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciShowRefutations(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (value == "true") {
		this->sicarioConfigs.uciShowRefutations = true;
	} else if (value == "false") {
		this->sicarioConfigs.uciShowRefutations = false;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciLimitStrength(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (value == "true") {
		this->sicarioConfigs.uciLimitStrength = true;
	} else if (value == "false") {
		this->sicarioConfigs.uciLimitStrength = false;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciElo(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (!isPostiveInteger(value)) {
		sendInvalidArgument(inputs);
		return;
	}

	if (std::stoi(value) < std::stoi(this->uciElo.min) || std::stoi(value) > std::stoi(this->uciElo.max)) {
		sendArgumentOutOfRange(inputs);
		return;
	}
	this->sicarioConfigs.uciElo = std::stoi(value);
}

void Sicario::setOptionUciAnalyseMode(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	if (value == "true") {
		this->sicarioConfigs.uciAnalyseMode = true;
	} else if (value == "false") {
		this->sicarioConfigs.uciAnalyseMode = false;
	} else {
		sendInvalidArgument(inputs);
	}
}

void Sicario::setOptionUciOpponent(const std::vector<std::string>& inputs) {
	std::string value = getOptionValue(inputs);
	std::vector<std::string> values = split(value, " ");
	if (values.size() < 4 || !isValidTitle(values[0]) || !isValidElo(values[1]) || !isValidPlayerType(values[2])) {
		sendInvalidArgument(inputs);
		return;
	}
	this->sicarioConfigs.uciOpponent = value;
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