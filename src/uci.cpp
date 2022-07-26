
#include <iostream>
#include <thread>
#include <algorithm>


#include "game.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"
#include "mcts.hpp"

void showEog(ExitCode code) {
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

/**
 * Parses the GUI input string and calls handlers for commands.
 * @param input: Input string from GUI.
 */
void Sicario::processInput(std::string& input) {
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
    }
}

UciInput Sicario::hashCommandInput(std::string& input) {
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

    return INVALID_COMMAND;
}

ConfigOption Sicario::hashOptionsInput(std::vector<std::string>& inputs) {
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

/**
 * Sends the response to the Uci engine after recieving the initial uci command.
 */
void Sicario::handleUci() {
    // ID information
    communicate("id name " + (std::string)NAME + " (" + (std::string)CODENAME + " " + (std::string)VERSION + ")");
    communicate("id author " + (std::string)AUTHOR + "\n");

    // Configurable options (see SicarioConfigs struct)
    sendOption(thread);
    sendOption(hash);
    sendOption(ponder);
    sendOption(ownBook);
    sendOption(multiPv);
    sendOption(uciShowCurrLine);
    sendOption(uciShowRefutations);
    sendOption(uciLimitStrength);
    sendOption(uciElo);
    sendOption(uciAnalyseMode);
    sendOption(uciOpponent);

    communicate("uciok");
}

void Sicario::handleIsReady() {
    sendReadyOk();
}

void Sicario::handleDebug(std::vector<std::string>& inputs) {
    if (inputs.size() != 2) {
        sendMissingArgument(inputs);
    } else if (inputs[1] != "on" && inputs[1] != "off") {
        sendInvalidArgument(inputs);
    } else if (inputs.size() == 2 && inputs[1] == "on") {
        sicarioConfigs.debugMode = true;
    } else if (inputs.size() == 2 && inputs[1] == "off") {
        sicarioConfigs.debugMode = false;
    }
}

void Sicario::handleSetOption(std::vector<std::string>& inputs) {
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

void Sicario::handlePosition(std::vector<std::string>& inputs) {
    // TODO play out moves as well and handle startpos command and unknown commands
    if (inputs[1] == "fen") {
        this->position.parseFen(concatFEN(inputs));
    }
}

void Sicario::handleGo(std::vector<std::string>& commands) {
    // TODO Parse search parameters

    if (searchTree == false) {
        searchTree = true;
        std::thread searchThread(&Sicario::mcts, this);
        searchThread.detach();
    }
}

void Sicario::handleStop() {
    searchTree = false;
}

void Sicario::handlePonderHit() {
    // TODO
}

void Sicario::handlePerft(std::vector<std::string>& commands) {
    uint64_t totalNodes = perft(std::stoi(commands[1]), true);
    std::cout << "Nodes searched: " << totalNodes << '\n';
}

void Sicario::handleMove(std::vector<std::string>& commands) {
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

    MoveList moves = MoveList(position);
    if (!moves.contains(move) && !moves.contains(move | EN_PASSANT)) { // NOTE en-passant checks are hacky...
        communicate("Invalid move");
        return;
    }

    if (moves.contains(move)) {
        position.processMakeMove(move);
    } else {
        position.processMakeMove(move | EN_PASSANT);
    }
}

void Sicario::handleUndo() {
    position.processUndoMove();
}

void Sicario::handleDisplay() {
    position.display();
}

void Sicario::handleMoves() {
    MoveList moves = MoveList(position);

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
    position.displayBitboards();
}

void Sicario::handleRandom() {
    position.display();

    for (int i = 0; i < 1000000; i++) {
        int moveCount = 0;

        std::cout << "here1" << '\n';
        MoveList moves = MoveList(position);
        std::cout << "here2" << '\n';
        std::vector<Move> playedMoves;
        while (!position.isEOG(moves)) {
            Move move = moves.randomMove();
            playedMoves.push_back(move);
            position.processMakeMove(move);
            // position.display();
            moves = MoveList(position);
            moveCount++;
        }
        std::cout << "here3" << '\n';
        showEog(position.isEOG(moves));
        std::cout << "here4" << '\n';
        while (moveCount > 0) {
            position.processUndoMove();
            moveCount--;
        }
        std::cout << "here5" << '\n';

        for (Move move : playedMoves) {
            printMove(move, false);
            std::cout << ' ';
        }
        std::cout << " " << i << "\n\n";
    }
}

/**
 * Prints all messages to Uci communication to stdin. Used to distinguish from other print commands.
 * @param communication: The communication message to be sent to Uci GUI.
 */
void Sicario::communicate(std::string communication) {
    std::cout << communication << '\n';
}

void Sicario::sendOption(OptionInfo& option) {
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

void Sicario::sendInvalidCommand(std::vector<std::string>& commands) {
    communicate("Unknown command: " + commands[0]);
}

void Sicario::sendMissingArgument(std::vector<std::string>& inputs) {
    communicate("Missing argument: " + concat(inputs, " "));
}

void Sicario::sendInvalidArgument(std::vector<std::string>& inputs) {
    communicate("Invalid argument: " + concat(inputs, " "));
}

void Sicario::sendUnknownOption(std::vector<std::string>& inputs) {
    communicate("Unknown option: " + getOptionName(inputs));
}

void Sicario::sendInvalidValue(std::vector<std::string>& inputs) {
    communicate("Invalid argument: " + getOptionValue(inputs));
}

void Sicario::sendArgumentOutOfRange(std::vector<std::string>& inputs) {
    communicate("Argument out of range: " + getOptionValue(inputs));
}

void Sicario::setOptionThread(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (!isPostiveInteger(value)) {
        sendInvalidArgument(inputs);
        return;
    }

    if (std::stoi(value) < std::stoi(thread.min) || std::stoi(value) > std::stoi(thread.max)) {
        sendArgumentOutOfRange(inputs);
        return;
    }
    sicarioConfigs.hash = std::stoi(value);
}

void Sicario::setOptionHash(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (!isPostiveInteger(value)) {
        sendInvalidArgument(inputs);
        return;
    }

    if (std::stoi(value) < std::stoi(hash.min) || std::stoi(value) > std::stoi(hash.max)) {
        sendArgumentOutOfRange(inputs);
        return;
    }
    sicarioConfigs.hash = std::stoi(value);
}

void Sicario::setOptionClearHash() {
    // TODO
}

void Sicario::setOptionPonder(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (value == "true") {
        sicarioConfigs.ponder = true;
    } else if (value == "false") {
        sicarioConfigs.ponder = false;
    } else {
        sendInvalidArgument(inputs);
    }
}

void Sicario::setOptionOwnBook(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (value == "true") {
        sicarioConfigs.ownBook = true;
    } else if (value == "false") {
        sicarioConfigs.ownBook = false;
    } else {
        sendInvalidArgument(inputs);
    }
}

void Sicario::setOptionMultiPV(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (!isPostiveInteger(value)) {
        sendInvalidArgument(inputs);
        return;
    }

    if (std::stoi(value) < std::stoi(multiPv.min) || std::stoi(value) > std::stoi(multiPv.max)) {
        sendArgumentOutOfRange(inputs);
        return;
    }
    sicarioConfigs.multiPv = std::stoi(value);
}

void Sicario::setOptionUciShowCurrLine(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (value == "true") {
        sicarioConfigs.uciShowCurrLine = true;
    } else if (value == "false") {
        sicarioConfigs.uciShowCurrLine = false;
    } else {
        sendInvalidArgument(inputs);
    }
}

void Sicario::setOptionUciShowRefutations(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (value == "true") {
        sicarioConfigs.uciShowRefutations = true;
    } else if (value == "false") {
        sicarioConfigs.uciShowRefutations = false;
    } else {
        sendInvalidArgument(inputs);
    }
}

void Sicario::setOptionUciLimitStrength(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (value == "true") {
        sicarioConfigs.uciLimitStrength = true;
    } else if (value == "false") {
        sicarioConfigs.uciLimitStrength = false;
    } else {
        sendInvalidArgument(inputs);
    }
}

void Sicario::setOptionUciElo(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (!isPostiveInteger(value)) {
        sendInvalidArgument(inputs);
        return;
    }

    if (std::stoi(value) < std::stoi(uciElo.min) || std::stoi(value) > std::stoi(uciElo.max)) {
        sendArgumentOutOfRange(inputs);
        return;
    }
    sicarioConfigs.uciElo = std::stoi(value);
}

void Sicario::setOptionUciAnalyseMode(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    if (value == "true") {
        sicarioConfigs.uciAnalyseMode = true;
    } else if (value == "false") {
        sicarioConfigs.uciAnalyseMode = false;
    } else {
        sendInvalidArgument(inputs);
    }
}

void Sicario::setOptionUciOpponent(std::vector<std::string>& inputs) {
    std::string value = getOptionValue(inputs);
    std::vector<std::string> values = split(value, " ");
    if (values.size() < 4 || !isValidTitle(values[0]) || !isValidElo(values[1]) || !isValidPlayerType(values[2])) {
        sendInvalidArgument(inputs);
        return;
    }
    sicarioConfigs.uciOpponent = value;
}

std::string Sicario::getOptionName(std::vector<std::string>& inputs) {
    auto nameItr = std::find(inputs.begin(), inputs.end(), "name");
    auto valueItr = std::find(inputs.begin(), inputs.end(), "value");
    unsigned long nameIndex = nameItr - inputs.begin();

    if (nameItr == inputs.end() || nameIndex != 1 || nameIndex + 1 > inputs.size() - 1) return "";
    std::string name = concat(std::vector<std::string>(nameItr + 1, valueItr), " ");
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
    return name;
}

std::string Sicario::getOptionValue(std::vector<std::string>& inputs) {
    auto nameItr = std::find(inputs.begin(), inputs.end(), "name");
    auto valueItr = std::find(inputs.begin(), inputs.end(), "value");
    unsigned long valueIndex = valueItr - inputs.begin();

    if (nameItr == inputs.end() || valueItr == inputs.end() || valueIndex + 1 > inputs.size() - 1) return "";
    return concat(std::vector<std::string>(valueItr + 1, inputs.end()), " ");
}