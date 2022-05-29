
#include <iostream>
#include <thread>

#include "game.hpp"
#include "uci.hpp"

#define NULL_MOVE 0000

/**********************************************************************************************************************/
/***************************************** Engine to interface commands ***********************************************/

/**
 * Prints all messages to Uci communication to stdin. Used to distinguish from other print commands.
 * @param communication: The communication message to be sent to Uci GUI.
 */
void communicate(std::string communication) {
    std::cout << communication << '\n' << std::flush;
}

void Uci::sendIds() {
    communicate("id name " + (std::string) NAME + " (" + (std::string) VERSION + ")");
    communicate("id author " + (std::string) AUTHOR + "\n");
}

void showOption(std::string name, std::string type, std::string def, std::string min = "", std::string max = "",
        std::vector<std::string> vars = {}) {
    std::cout << "option name " << name << " type " << type << " default " << def;
    if (min != "") std::cout << " min " << min << " max " << max;
    for (std::string var : vars) std::cout << " var " << var;
    std::cout << '\n';
}

void Uci::options() {
    showOption("Exploration Constant", "spin", "default_const", "-infinity", "infinity", {"default_constant"});
}

/**
 * Sends the response to the Uci engine after recieving the initial uci command.
 */
void Uci::sendInitialResponse() {
    this->sendIds();
    this->options();
    communicate("uciok");
}

/**
 * Sends the is readyok.
 */
void Uci::sendReadyOk() {
    communicate("readyok");
}

void Uci::sendBestMove() {

}

void Uci::sendCopyProtection() {

}

void Uci::sendRegistration() {

}

void Uci::sendInfo() {

}

/************************************** End of engine to interface commands *******************************************/
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/***************************************** Interface to engine commands ***********************************************/

void Uci::handleIsReady() {
    this->sendReadyOk();
}

void Uci::handleDebug(std::vector<std::string> inputs) {
    communicate("handle debug");
    if (inputs.size() == 2 && inputs[1] == "on") {

    } else if (inputs.size() == 2 && inputs[1] == "off") {

    }
}

void Uci::handleSetOption(std::vector<std::string> inputs) {
    if (inputs[2] == "Exploration" && inputs[3] == "Constant") {
        this->params.c = std::stof(inputs[4]); // TODO Set the default variable options for this one.
    } else {
        std::cout << "Unknown setoption " << inputs[2] << "\n"; // TODO Print the rest of the commands
    }
    std::cout << this->params.c << "\n";
}

void Uci::handleRegister(std::vector<std::string> inputs) {
    communicate("handle register");
}

void Uci::handleUCI_NewGame() {
    this->pos.parseFen(STANDARD_GAME);
}

void Uci::handlePosition(std::vector<std::string> inputs) {
    // Insert dummy command at start to use concatFEN
    inputs.insert(inputs.begin(), "set");
    this->pos.parseFen(concatFEN(inputs));
}

void Uci::handleGo(std::vector<std::string> commands) {
    GoParams go_params;

    // Parse go commands
    for (int i = 1; i < (int) commands.size(); i++) { // TODO Error checking needed
        if (commands[i] == "searchmoves") {
            for (int j = i + 1; j < (int) commands.size(); j++) {
                Move move = Uci::parseMove(commands[j]);
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

    std::thread searchThread(&Position::search, &this->pos, this->params, go_params);
    searchThread.detach();
}

Move Uci::parseMove(std::string move_str) {
    Move move = 0;
    if (move_str[0] < 'a' || move_str[0] > 'h' || move_str[2] < 'a' || move_str[2] > 'h') {
        return 0;
    }

    if (move_str[1] < '1' || move_str[1] > '8' || move_str[3] < '1' || move_str[3] > '8') {
        return 0;
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
    return move;
}

void Uci::handleStop() {
    // stop = true;
}

void Uci::handlePonderHit() {
    communicate("handle ponderhit");
}

/************************************** End of interface to engine commands *******************************************/
/**********************************************************************************************************************/



/**
 * The main loop that constantly reads from stdin for Uci commands. Calls other functions that create threads which
 * handle different parts of program.
 * @param input: THe initial input string.
 */
void Uci::parseInput(std::string input) {
    std::vector<std::string> commands = split(input, " ");
    UciInputs hashedInput = hashInput(commands[0]);

    switch (hashedInput) {
        case QUIT:
            break;
        case INVALID_INPUT:
            // TODO
            std::cout << "Invalid input. This needs to be updated to parse the remainder of the string (Uci protocol)" << '\n';
            break;
        case UCI:
            // TODO
            std::cout << "todo" << '\n';
            break;
        case ISREADY:
            handleIsReady();
            break;
        case SETOPTION:
            handleSetOption(commands);
            break;
        case DEBUG:
            handleDebug(commands);
            break;
        case REGISTER:
            handleRegister(commands);
            break;
        case UCINEWGAME:
            handleUCI_NewGame();
            break;
        case POSITION:
            handlePosition(commands);
            break;
        case GO:
            // handleGo(commands);
            break;
        case STOP:
            std::cout << "TODO stopping" << '\n';
            break;
        case PONDERHIT:
            handlePonderHit();
            break;
    }
}

UciInputs Uci::hashInput(std::string input) {
    if (input == "quit") return QUIT;
    if (input == "uci") return UCI;
    if (input == "isready") return ISREADY;
    if (input == "setoption") return SETOPTION;
    if (input == "debug") return DEBUG;
    if (input == "register") return REGISTER;
    if (input == "ucinewgame") return UCINEWGAME;
    if (input == "position") return POSITION;
    if (input == "go") return GO;
    if (input == "stop") return STOP;
    if (input == "ponderhit") return PONDERHIT;

    return INVALID_INPUT;
}
