
#include <iostream>
#include <thread>
#include <algorithm>


#include "game.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

inline bool isValidTitle(std::string str) {
    return str == "GM" || str == "IM" || str == "FM" || str == "WGM" || str == "WIM" || str == "none";
}

inline bool isValidPlayerType(std::string str) {
    return str == "computer" || str == "human";
}

inline bool isValidElo(std::string str) {
    return isNumber(str) || str == "none";
}

/**
 * Parses the GUI input string and calls handlers for commands.
 * @param input: Input string from GUI.
 */
void Sicario::processInput(std::string& input) {
    std::vector<std::string> commands = split(input, " ");
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
            sendInvalidCommand(commands[0]);
            break;
        case PERFT:
            handlePerft(commands[0]);
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
    position.parseFen(STANDARD_GAME);
}

void Sicario::handlePosition(std::vector<std::string>& inputs) {
    // Insert dummy command at start to use concatFEN
    // CHECK
    inputs.insert(inputs.begin(), "set");
    this->position.parseFen(concatFEN(inputs));
}

void Sicario::handleGo(std::vector<std::string>& commands) {
    // TODO
    GoParams go_params;

    // Parse go commands
    for (int i = 1; i < (int) commands.size(); i++) { // TODO Error checking needed
        if (commands[i] == "searchmoves") {
            for (int j = i + 1; j < (int) commands.size(); j++) {
                Move move = parseMove(commands[j]);
                if (move == 0) continue;
                go_params.moves.push_back(move);
            }
        } else if (commands[i] == "ponder") {
            // TODO
        } else if (commands[i] == "wtime") {
            go_params.wtime = stoi(commands[i + 1]);
        } else if (commands[i] == "btime") {
            go_params.btime = stoi(commands[i + 1]);
        } else if (commands[i] == "winc") {
            go_params.winc = stoi(commands[i + 1]);
        } else if (commands[i] == "binc") {
            go_params.binc = stoi(commands[i + 1]);
        } else if (commands[i] == "movestogo") {
            go_params.moves_to_go = stoi(commands[i + 1]);
        } else if (commands[i] == "depth") {
            go_params.depth = stoi(commands[i + 1]);
        } else if (commands[i] == "nodes") {
            go_params.nodes = stoi(commands[i + 1]);
        } else if (commands[i] == "mate") {
            go_params.mate = stoi(commands[i + 1]);
        } else if (commands[i] == "movetime") {
            go_params.movetime = stoi(commands[i + 1]);
        } else if (commands[i] == "infinite") {
            go_params.infinite = true;
        }
    }

    std::thread searchThread(&Position::search, &this->position, go_params);
    searchThread.detach();
}

void Sicario::handleStop() {
    // TODO
}

void Sicario::handlePonderHit() {
    // TODO
}

void Sicario::handlePerft(std::string& depth) {
    // TODO
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

void Sicario::sendInvalidCommand(std::string& command) {
    communicate("Unknown command: " + command);
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
    if (!isNumber(value)) {
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
    if (!isNumber(value)) {
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
    if (!isNumber(value)) {
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
    if (!isNumber(value)) {
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