
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>

#include "game.hpp"
#include "uci.hpp"

#define NULL_MOVE 0000

/**********************************************************************************************************************/
/***************************************** Engine to interface commands ***********************************************/

/**
 * Prints all messages to UCI communication to stdin. Used to distinguish from other print commands.
 * @param communication: The communication message to be sent to UCI GUI.
 */
void communicate(std::string communication) {
    std::cout << communication << '\n' << std::flush;
}

void UCI::sendIds() {
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

void UCI::options() {
    showOption("Exploration Constant", "spin", "default_const", "-infinity", "infinity", {"default_constant"});
}

/**
 * Sends the response to the UCI engine after recieving the initial uci command.
 */
void UCI::sendInitialResponse() {
    this->sendIds();
    this->options();
    communicate("uciok");
}

/**
 * Sends the is readyok.
 */
void UCI::sendReadyOk() {
    communicate("readyok");
}

void UCI::sendBestMove() {

}

void UCI::sendCopyProtection() {

}

void UCI::sendRegistration() {
    
}

void UCI::sendInfo() {

}

/************************************** End of engine to interface commands *******************************************/
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/***************************************** Interface to engine commands ***********************************************/

void UCI::handleIsReady() {
    this->sendReadyOk();
}

void UCI::handleDebug(std::vector<std::string> inputs) {
    communicate("handle debug");
    if (inputs.size() == 2 && inputs[1] == "on") {

    } else if (inputs.size() == 2 && inputs[1] == "off") {

    }
}

void UCI::handleSetOption(std::vector<std::string> inputs) {
    if (inputs[2] == "Exploration" && inputs[3] == "Constant") {
        this->params.c = std::stof(inputs[4]); // TODO Set the default variable options for this one.
    } else {
        std::cout << "Unknown setoption " << inputs[2] << "\n"; // TODO Print the rest of the commands
    }
    std::cout << this->params.c << "\n";
}

void UCI::handleRegister(std::vector<std::string> inputs) {
    communicate("handle register");
}

void UCI::handleUCI_NewGame() {
    this->pos.parseFen(STANDARD_GAME);
}

void UCI::handlePosition(std::vector<std::string> inputs) {
    // Insert dummy command at start to use concatFEN
    inputs.insert(inputs.begin(), "set");
    this->pos.parseFen(concatFEN(inputs));
}

void UCI::handleGo(std::vector<std::string> commands, std::atomic_bool& stop) {
    if (stop != true) return;
    GoParams go_params;

    // Parse go commands
    for (int i = 1; i < (int) commands.size(); i++) { // TODO Error checking needed
        if (commands[i] == "searchmoves") {
            for (int j = i + 1; j < (int) commands.size(); j++) {
                Move move = UCI::parseMove(commands[j]);
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

    for (Move move : go_params.moves) {
        printMove(move, true);
        std::cout << "\n";
    }

    stop = false;
    std::thread searchThread(&Pos::search, &this->pos, this->params, std::ref(stop), go_params);
    searchThread.detach();
}

Move UCI::parseMove(std::string move_str) {
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

void UCI::handleStop(std::atomic_bool& stop) {
    stop = true;
}

void UCI::handlePonderHit() {
    communicate("handle ponderhit");
}

/************************************** End of interface to engine commands *******************************************/
/**********************************************************************************************************************/



/**
 * The main loop that constantly reads from stdin for UCI commands. Calls other functions that create threads which
 * handle different parts of program.
 * @param input: THe initial input string.
 */
void UCI::runUCI(std::string input) {
    this->sendInitialResponse();
    std::atomic_bool stop(true);
    while (input != "quit" && input != "exit" && input != "q") {
        std::vector<std::string> commands = split(input, " ");
        if (commands[0] == "isready")           this->handleIsReady();
        else if (commands[0] == "setoption")    this->handleSetOption(commands);
        else if (commands[0] == "debug")        this->handleDebug(commands);
        else if (commands[0] == "register")     this->handleRegister(commands);
        else if (commands[0] == "ucinewgame")   this->handleUCI_NewGame();
        else if (commands[0] == "position")     this->handlePosition(commands);
        else if (commands[0] == "go")           this->handleGo(commands, stop);
        else if (commands[0] == "stop")         this->handleStop(stop);
        else if (commands[0] == "ponderhit")    this->handlePonderHit();
        std::cout << std::flush;
        std::getline(std::cin, input);
    }
    this->handleStop(stop);
}

/**
 * Initialisation to start reading from stdin. Main function that starts main thread of the program and handles all UCI
 * interations.
 * @param input: THe initial input string.
 */
void UCI::init(std::string input) {
    UCI game_instance;
    game_instance.runUCI(input);
}
