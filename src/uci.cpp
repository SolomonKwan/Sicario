
#include <iostream>
#include <thread>
#include <algorithm>
#include <ranges>

#include "game.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

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
	return isNonNegativeInteger(str) || str == "none";
}

void Sicario::processInput(const std::string& input) {
	std::vector<std::string> commands = split(input, " ");
	if (commands.size() == 0)
		return;

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
		case HASHCOUNT:
			handleHash();
			break;
		case LETTERMODE:
			handleLetterMode(commands);
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
	if (input == "data") return DATA;
	if (input == "hash") return HASHCOUNT;
	if (input == "lettermode") return LETTERMODE;

	return INVALID_COMMAND;
}

void Sicario::parseGo(const std::vector<std::string>& inputs) {
	this->searchParams.reset();

	for (int index = 0; static_cast<size_t>(index) < inputs.size(); index++) {
		if (inputs[index] == "searchmoves") parseSearchmoves(index, inputs);
		else if (inputs[index] == "ponder") parsePonder(index, inputs);
		else if (inputs[index] == "wtime") parseWtime(index, inputs);
		else if (inputs[index] == "btime") parseBtime(index, inputs);
		else if (inputs[index] == "winc") parseWinc(index, inputs);
		else if (inputs[index] == "binc") parseBinc(index, inputs);
		else if (inputs[index] == "movestogo") parseMovestogo(index, inputs);
		else if (inputs[index] == "depth") parseDepth(index, inputs);
		else if (inputs[index] == "nodes") parseNodes(index, inputs);
		else if (inputs[index] == "mate") parseMate(index, inputs);
		else if (inputs[index] == "movetime") parseMovetime(index, inputs);
		else if (inputs[index] == "infinite") parseInfinite(index, inputs);
	}

	if (this->searchParams.movesToGo == -1 && this->searchParams.wtime >= 0 && this->searchParams.btime >= 0) {
		this->searchParams.suddenDeath = true;
	}
}

void Sicario::parseSearchmoves(int index, const std::vector<std::string>& inputs) {
	MoveList moves = MoveList(this->position);
	for (auto moveIndex = index + 1; static_cast<size_t>(moveIndex) < inputs.size(); moveIndex++)
		if (moves.contains(this->position.getPositionMove(inputs[moveIndex])))
			this->searchParams.searchMoves.push_back(getMove(inputs[moveIndex]));
}

void Sicario::parsePonder(int index, const std::vector<std::string>& inputs) {
	this->searchParams.ponder = true;
}

void Sicario::parseWtime(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isNonNegativeInteger(inputs[index + 1]))
		return;
	this->searchParams.wtime = std::stoi(inputs[index + 1]);
}

void Sicario::parseBtime(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isNonNegativeInteger(inputs[index + 1]))
		return;
	this->searchParams.btime = std::stoi(inputs[index + 1]);
}

void Sicario::parseWinc(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isPositiveInteger(inputs[index + 1]))
		return;
	this->searchParams.winc = std::stoi(inputs[index + 1]);
}

void Sicario::parseBinc(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isPositiveInteger(inputs[index + 1]))
		return;
	this->searchParams.binc = std::stoi(inputs[index + 1]);
}

void Sicario::parseMovestogo(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isPositiveInteger(inputs[index + 1]))
		return;
	this->searchParams.movesToGo = std::stoi(inputs[index + 1]);
}

void Sicario::parseDepth(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isNonNegativeInteger(inputs[index + 1]))
		return;
	this->searchParams.depth = std::stoi(inputs[index + 1]);
}

void Sicario::parseNodes(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isNonNegativeInteger(inputs[index + 1]))
		return;
	this->searchParams.nodes = std::stoi(inputs[index + 1]);
}

void Sicario::parseMate(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isNonNegativeInteger(inputs[index + 1]))
		return;
	this->searchParams.mate = std::stoi(inputs[index + 1]);
}

void Sicario::parseMovetime(int index, const std::vector<std::string>& inputs) {
	if (static_cast<size_t>(index + 1) >= inputs.size() || !isNonNegativeInteger(inputs[index + 1]))
		return;
	this->searchParams.moveTime = std::stoi(inputs[index + 1]);
}

void Sicario::parseInfinite(int index, const std::vector<std::string>& inputs) {
	this->searchParams.infinite = true;
}

void Sicario::handleUci() {
	Uci::sendId();
	Uci::sendOption(this->options.ThreadOption);
	Uci::sendOption(this->options.HashOption);
	Uci::sendOption(this->options.PonderOption);
	Uci::sendOption(this->options.OwnBookOption);
	Uci::sendOption(this->options.MultiPVOption);
	Uci::sendOption(this->options.UCIShowRefutationsOption);
	Uci::sendOption(this->options.UCILimitStrengthOption);
	Uci::sendOption(this->options.UCIEloOption);
	Uci::sendOption(this->options.UCIAnalyseModeOption);
	Uci::sendOption(this->options.UCIOpponentOption);
	Uci::sendOption(this->options.ClearHashOption);
	Uci::sendOption(this->options.ExplorationOption);
	Uci::sendUciok();
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
		this->options.debugMode = true;
	} else if (inputs.size() == 2 && inputs[1] == "off") {
		this->options.debugMode = false;
	}
}

void Sicario::handleSetOption(const std::vector<std::string>& inputs) {
	const auto[ name, value ] = getOptionNameAndValue(inputs);

	if (name == "thread") return setSpinOption(this->options.ThreadOption, value);
	else if (name == "hash") return setSpinOption(this->options.HashOption, value);
	else if (name == "ponder") return setCheckOption(this->options.PonderOption, value);
	else if (name == "ownbook") return setCheckOption(this->options.OwnBookOption, value);
	else if (name == "multipv") return setSpinOption(this->options.MultiPVOption, value);
	else if (name == "uci_showrefutations") return setCheckOption(this->options.UCIShowRefutationsOption, value);
	else if (name == "uci_limitstrength") return setCheckOption(this->options.UCILimitStrengthOption, value);
	else if (name == "uci_elo") return setSpinOption(this->options.UCIEloOption, value);
	else if (name == "uci_analysemode") return setCheckOption(this->options.UCIAnalyseModeOption, value);
	else if (name == "uci_opponent") return setUciOpponentOption(this->options.UCIOpponentOption, value);
	else if (name == "clearhash") return this->clearCache();
	else if (name == "exploration") return this->setSpinOption(this->options.ExplorationOption, value);
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

	MoveList moves = MoveList(this->position);
	while (++ptr != inputs.end()) {
		if (moves.contains(this->position.getPositionMove(*ptr)))
			this->position.makeMove(this->getPosition().getPositionMove(*ptr));
	}
}

void Sicario::handleGo(const std::vector<std::string>& inputs) {
	this->parseGo(inputs);
	if (this->searchTree == false) {
		this->searchTree = true;
		this->threads.push_back(std::thread(&Sicario::search, this));
	}
}

void Sicario::handleStop() {
	this->searchTree = false;
}

void Sicario::handlePonderHit() {
	this->searchParams.ponder = false;
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

	if (this->options.debugMode) {
		std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "μs\n";
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms\n";
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(finish - start).count() << "s\n";
	}
}

void Sicario::handleMove(const std::vector<std::string>& inputs) {
	Move move = this->getPosition().getPositionMove(inputs[1]);
	if (move == NULL_MOVE) {
		std::cerr << "Invalid move: " + inputs[1] << '\n';
		return;
	}

	MoveList moves = MoveList(this->position);
	if (!moves.contains(move)) {
		std::cerr << "Invalid move: " + inputs[1]  << '\n';
		return;
	}

	this->position.makeMove(move);
}

void Sicario::handleUndo() {
	this->position.undoMove();
}

void Sicario::handleDisplay() {
	this->position.display(this->options.letterMode);
}

void Sicario::handleMoves() {
	MoveList moves = MoveList(this->position);
	std::cout << "There are " << moves.size() << " moves" << '\n';
	for (Move move : moves)
		std::cout << getMove(move, true) << '\n';
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
			this->position.makeMove(move);
			moves = MoveList(this->position);
			moveCount++;
		}
		results[this->position.isEOG(moves)]++;

		// Undo moves back to original position.
		while (moveCount > 0) {
			this->position.undoMove();
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
	std::cout << this->options.ThreadOption.toString() << '\n';
	std::cout << this->options.HashOption.toString() << '\n';
	std::cout << this->options.PonderOption.toString() << '\n';
	std::cout << this->options.OwnBookOption.toString() << '\n';
	std::cout << this->options.MultiPVOption.toString() << '\n';
	std::cout << this->options.UCIShowRefutationsOption.toString() << '\n';
	std::cout << this->options.UCILimitStrengthOption.toString() << '\n';
	std::cout << this->options.UCIEloOption.toString() << '\n';
	std::cout << this->options.UCIAnalyseModeOption.toString() << '\n';
	std::cout << this->options.UCIOpponentOption.toString() << '\n';
	std::cout << this->options.ExplorationOption.toString() << '\n';
}

void Sicario::handleData() {
	// Print the move history.
	const std::vector<History>& historyVec = this->getPosition().getHistory();
	for (const History& history : historyVec) {
		std::cout << getMove(history.move);
		if (&history != &historyVec.back()) std::cout << ' ';
	}
	std::cout << '\n';

	// Print the positionCounts size.
	std::cout << this->getPosition().getPositionCounts().size() << '\n';
}

void Sicario::handleHash() {
	Hash hash = this->getPosition().getHash();
	std::cout << hash << '\n';
	std::cout << this->getPosition().getPositionCounts().find(hash)->second << '\n';
}

void Sicario::handleLetterMode(const std::vector<std::string>& inputs) {
	if (inputs.size() != 2)
		sendMissingArgument(inputs);
	else if (inputs[1] != "on" && inputs[1] != "off")
		sendInvalidArgument(inputs[1]);
	else if (inputs.size() == 2 && inputs[1] == "on")
		this->options.letterMode = true;
	else if (inputs.size() == 2 && inputs[1] == "off")
		this->options.letterMode = false;
}

void Uci::send(std::string message) {
	std::cout << message << '\n';
}

void Uci::sendId() {
	Uci::send("id name " + NAME + " (" + CODENAME + " " + VERSION + ")");
	Uci::send("id author " + AUTHOR + "\n");
}

void Uci::sendUciok() {
	Uci::send("uciok");
}

void Uci::sendReadyOk() {
	Uci::send("readyok");
}

void Uci::sendBestMove(Node* root, bool debugMode) {
	assert(root->bestChild() != nullptr);

	// Bestmove
	Node* bestChild = root->bestChild();
	std::string message = "bestmove " + getMove(bestChild->getInEdge());

	// Pondermove
	Node* ponderNode = bestChild->bestChild();
	if (ponderNode != nullptr)
		message += " ponder " + getMove(ponderNode->getInEdge());

	Uci::send(message);

	if (!debugMode) return;

	std::vector<float> ucb1Values;
	for (auto child : root->getChildren())
		ucb1Values.push_back(child->Ucb1());
	std::vector<size_t> ucbRanks = rankSort(ucb1Values);

	for (auto child : root->getChildren()) {
		std::cout << getMove(child->getInEdge(), true);
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

void Uci::sendInfo(SearchInfo& searchInfo, Node* root, const Option& options) {
	for (int pvLine = 1; pvLine <= options.MultiPVOption.getValue(); pvLine++) {
		std::string infoMessage = "info ";
		infoMessage += Info::depth(searchInfo) + " ";
		infoMessage += Info::selDepth(searchInfo) + " ";
		infoMessage += Info::time(searchInfo) + " ";
		infoMessage += Info::nodes(searchInfo) + " ";
		infoMessage += Info::multiPv(root, pvLine) + " ";
		infoMessage += Info::pv(root, pvLine) + " ";
		infoMessage += Info::score(root) + " ";
		infoMessage += Info::currMove(searchInfo, root) + " ";
		infoMessage += Info::currMoveNumber(searchInfo, root) + " ";
		infoMessage += Info::hashfull(searchInfo) + " ";
		infoMessage += Info::nps(searchInfo);
		Uci::send(infoMessage);
	}
	searchInfo.setPrintInfo(false);
}

template<typename T>
void Uci::sendOption(const OptionConfig<T>& option) {
	Uci::send(option.toUciString());
}

std::string Info::depth(SearchInfo& searchInfo) {
	return "depth " + std::to_string(searchInfo.getDepth());
}

std::string Info::selDepth(SearchInfo& searchInfo) {
	return "seldepth " + std::to_string(searchInfo.getSeldepth());
}

std::string Info::time(SearchInfo& searchInfo) {
	return "time xxxx";
}

std::string Info::nodes(SearchInfo& searchInfo) {
	return "nodes " + std::to_string(searchInfo.getNodes());
}

std::string Info::pv(Node* root, int pvLine) {
	std::string pv = "pv ";
	Node* curr = root->bestChildPv(pvLine);
	while (curr != nullptr) {
		pv += getMove(curr->getInEdge());
		curr = curr->bestChild();
		if (curr != nullptr)
			pv += " ";
	}
	return pv;
}

std::string Info::multiPv(Node* root, int pvLine) {
	if (pvLine > static_cast<int>(root->getChildren().size())) return "";
	return "multipv " + std::to_string(pvLine);
}

std::string Info::score(Node* root) {
	std::string scoreStr = "score ";
	// if (root->getMateDepth() != 0) {
	// 	scoreStr += "mate " + Info::mate(root);
	// } else {
	// 	float score = 0;
	// 	for (auto child : root->getChildren()) {
	// 		float childScore = child->Ucb1();
	// 		if (childScore == std::numeric_limits<float>::max()) continue;
	// 		score += child->Ucb1();
	// 	}
	// 	scoreStr += "cp " + Info::cp(root);
	// }

	scoreStr += Info::lowerBound(root);
	scoreStr += Info::upperBound(root);

	return scoreStr;
}

std::string Info::mate(Node* root) {
	// return std::to_string(root->getMateDepth());
	return "";
}

std::string Info::cp(Node* root) {
	float score = 0;
	for (auto child : root->getChildren()) {
		float childScore = child->Ucb1();
		if (childScore == std::numeric_limits<float>::max()) continue;
		score += child->Ucb1();
	}
	return std::to_string(convertToCentipawn(score / root->getChildren().size()));
}

std::string Info::lowerBound(Node* root) {
	if (true) // TODO calculate condition
		return " lowerbound";
	return "";
}

std::string Info::upperBound(Node* root) {
	if (true) // TODO calculate condition
		return " upperbound";
	return "";
}

std::string Info::currMove(SearchInfo& searchInfo, Node* root) {
	return "currmove " + getMove(searchInfo.getCurrMove());
}

std::string Info::currMoveNumber(SearchInfo& searchInfo, Node* root) {
	return "currmovenumber x";
}

std::string Info::hashfull(SearchInfo& searchInfo) {
	return "hashfull xxxx";
}

std::string Info::nps(SearchInfo& searchInfo) {
	// auto end = std::chrono::high_resolution_clock::now();
	// float npms = searchInfo.getNodes() * 1000;
	// npms /= std::chrono::duration_cast<std::chrono::milliseconds>(end - searchInfo.getStart()).count();
	// return "nps " + std::to_string(static_cast<uint>(npms));
	return "";
}

void Sicario::sendInvalidCommand(const std::vector<std::string>& inputs, const std::string& customMsg) {
	std::cerr << "Unknown command: " + (customMsg == "" ? inputs[0] : customMsg) << '\n';
}

void Sicario::sendMissingArgument(const std::vector<std::string>& inputs, const std::string& customMsg) {
	std::cerr << "Missing argument: " + (customMsg == "" ? concat(inputs, " ") : customMsg) << '\n';
}

void Sicario::sendInvalidArgument(const std::string& value, const std::string& customMsg) {
	std::cerr << "Invalid argument: " + (customMsg == "" ? value : customMsg) << '\n';
}

void Sicario::sendUnknownOption(const std::string& option, const std::string& customMsg) {
	std::cerr << "Unknown option: " + (customMsg == "" ? option : customMsg) << '\n';
}

void Sicario::sendInvalidValue(const std::string& value, const std::string& customMsg) {
	std::cerr << "Invalid argument: " + (customMsg == "" ? value : customMsg) << '\n';
}

void Sicario::sendArgumentOutOfRange(const std::string& value, const std::string& customMsg) {
	std::cerr << "Argument out of range: " + (customMsg == "" ? value : customMsg) << '\n';
}

void Sicario::setCheckOption(OptionConfig<bool>& option, const std::string& value) {
	if (value == "") return;
	if (value == "true" || value == "false") {
		option.setValue(value == "true" ? true : false);
	} else {
		sendInvalidArgument(value);
	}
}

void Sicario::setSpinOption(OptionConfig<int>& option, const std::string& value) {
	if (value == "") return;
	if (!isPositiveInteger(value) || std::stoi(value) < option.getMin() || std::stoi(value) > option.getMax()) {
		sendArgumentOutOfRange(value);
		return;
	}
	option.setValue(std::stoi(value));
}

void Sicario::setComboOption(OptionConfig<std::string>& option, const std::string& value) {
	std::vector<std::string>& vars = option.getVars();
	if (std::find(vars.begin(), vars.end(), value) != vars.end())
		return;
	option.setValue(value);
}

void Sicario::setStringOption(OptionConfig<std::string>& option, const std::string& value) {
	option.setValue(value);
}

void Sicario::setUciOpponentOption(OptionConfig<std::string>& option, const std::string& value) { // TODO might be able to abstract the logic check out to fn pointers thereby no need for individual option commands at all. Might be able to combine with the other sets for the other types
	std::vector<std::string> values = split(value, " ");
	if (value != "" && (values.size() < 4 || !isValidTitle(values[0]) || !isValidElo(values[1]) ||
			!isValidPlayerType(values[2]))) {
		sendInvalidArgument(value, "[GM|IM|FM|WGM|WIM|none] [<elo>|none] [computer|human] <name>");
		return;
	}
	option.setValue(value);
}

void Sicario::clearCache() {

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

int convertToCentipawn(float value) {
	return 5000.0 / (1.0 + std::exp(-6.0 * value)) - 2500;
}