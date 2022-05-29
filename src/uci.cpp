
#include <iostream>
#include <thread>
#include <algorithm>


#include "game.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

#define NULL_MOVE 0000

/**
 * Parses the GUI input string and calls handlers for commands.
 * @param input: Input string from GUI.
 */
void Sicario::processInput(std::string input) {
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
        case REGISTER:
            handleRegister(commands);
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

UciInput Sicario::hashCommandInput(std::string input) {
    // UCI protocol GUI to engine commands
    if (input == "uci") return UCI;
    if (input == "debug") return DEBUG;
    if (input == "isready") return ISREADY;
    if (input == "setoption") return SETOPTION;
    if (input == "register") return REGISTER;
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

SicarioOption Sicario::hashOptionsInput(std::vector<std::string> inputs) {
    auto nameItr = std::find(inputs.begin(), inputs.end(), "name");
    int nameIndex = nameItr - inputs.begin();

    if (nameItr == inputs.end()) return INVALID_OPTION_SYNTAX;
    if (nameIndex != 1) return INVALID_OPTION_SYNTAX;
    if (nameIndex + 1 > inputs.size() - 1) return NO_OPTION_GIVEN;
    if (inputs[nameIndex + 1] == "Hash") return HASH;
    if (inputs[nameIndex + 1] == "NalimovPath") return NALIMOV_PATH;
    if (inputs[nameIndex + 1] == "NalimovCache") return NALIMOV_CACHE;
    if (inputs[nameIndex + 1] == "Ponder") return PONDER;
    if (inputs[nameIndex + 1] == "OwnBook") return OWN_BOOK;
    if (inputs[nameIndex + 1] == "MultiPV") return MULTI_PV;
    if (inputs[nameIndex + 1] == "UCI_ShowCurrLine") return UCI_SHOW_CURR_LINE;
    if (inputs[nameIndex + 1] == "UCI_ShowRefutations") return UCI_SHOW_REFUTATIONS;
    if (inputs[nameIndex + 1] == "UCI_LimitStrength") return UCI_LIMIT_STRENGTH;
    if (inputs[nameIndex + 1] == "UCI_Elo") return UCI_ELO;
    if (inputs[nameIndex + 1] == "UCI_AnalyseMode") return UCI_ANALYSE_MODE;
    if (inputs[nameIndex + 1] == "UCI_Opponent") return UCI_OPPONENT;

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
    sendOption({"Hash", "spin", "16"});
    sendOption({"NalimovPath", "string", ""});
    sendOption({"NalimovCache", "spin", "0"});
    sendOption({"Ponder", "check", "false"});
    sendOption({"OwnBook", "check", "false"});
    sendOption({"MultiPV", "spin", "1"});
    sendOption({"UCI_ShowCurrLine", "check", "false"});
    sendOption({"UCI_ShowRefutations", "check", "false"});
    sendOption({"UCI_LimitStrength", "check", "false"});
    sendOption({"UCI_Elo", "spin", "3000"});
    sendOption({"UCI_AnalyseMode", "check", "true"});
    sendOption({"UCI_Opponent", "string", ""});

    communicate("uciok");
}

void Sicario::handleIsReady() {
    sendReadyOk();
}

void Sicario::handleDebug(std::vector<std::string> inputs) {
    if (inputs.size() != 2) {
        sendMissingArgument("debug [ on | off ]");
    } else if (inputs[1] != "on" && inputs[1] != "off") {
        sendInvalidArgument(inputs[1]);
    } else if (inputs.size() == 2 && inputs[1] == "on") {
        sicaroConfigs.DEBUG_MODE = true;
    } else if (inputs.size() == 2 && inputs[1] == "off") {
        sicaroConfigs.DEBUG_MODE = false;
    }
}

void Sicario::handleSetOption(std::vector<std::string> inputs) {
    SicarioOption hashedInput = hashOptionsInput(inputs);

    switch (hashedInput) {
        case HASH:
            break;
        case NALIMOV_PATH:
            break;
        case NALIMOV_CACHE:
            break;
        case PONDER:
            break;
        case OWN_BOOK:
            break;
        case MULTI_PV:
            break;
        case UCI_SHOW_CURR_LINE:
            break;
        case UCI_SHOW_REFUTATIONS:
            break;
        case UCI_LIMIT_STRENGTH:
            break;
        case UCI_ELO:
            break;
        case UCI_ANALYSE_MODE:
            break;
        case UCI_OPPONENT:
            break;
        case INVALID_OPTION_SYNTAX:
            sendInvalidOptionSyntax(concat(inputs, " "));
            break;
        case NO_OPTION_GIVEN:
            sendNoOptionGiven(concat(inputs, " "));
            break;
        case UNKNOWN_OPTION:
            sendUnknownOption(inputs[2]);
            break;
    }
}

void Sicario::handleRegister(std::vector<std::string> inputs) {
    // TODO Dafuq is this?
}

void Sicario::handleUciNewGame() {
    position.parseFen(STANDARD_GAME);
}

void Sicario::handlePosition(std::vector<std::string> inputs) {
    // Insert dummy command at start to use concatFEN
    // CHECK
    inputs.insert(inputs.begin(), "set");
    this->position.parseFen(concatFEN(inputs));
}

void Sicario::handleGo(std::vector<std::string> commands) {
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

void Sicario::handlePerft(std::string depth) {
    // TODO
}

/**
 * Prints all messages to Uci communication to stdin. Used to distinguish from other print commands.
 * @param communication: The communication message to be sent to Uci GUI.
 */
void Sicario::communicate(std::string communication) {
    std::cout << communication << '\n';
}

void Sicario::sendOption(OptionInfo option) {
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

void Sicario::sendInvalidCommand(std::string command) {
    communicate("Unknown command: " + command);
}

void Sicario::sendMissingArgument(std::string arg) {
    communicate("Missing argument: " + arg);
}

void Sicario::sendInvalidArgument(std::string arg) {
    communicate("Invalid argument: " + arg);
}

void Sicario::sendInvalidOptionSyntax(std::string input) {
    communicate("Invalid syntax: " + input);
}

void Sicario::sendNoOptionGiven(std::string input) {
    communicate("No option given: " + input);
}

void Sicario::sendUnknownOption(std::string option) {
    communicate("Unknown option: " + option);
}