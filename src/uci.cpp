
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>

#include "game.hpp"
#include "uci.hpp"

# define NULL_MOVE 0000

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
    communicate("id name " + (std::string) VERSION);
    communicate("id author " + (std::string) AUTHOR);
}

void UCI::options() {
    
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

void UCI::handleSetOption() {
    communicate("handle set option");
}

void UCI::handleDebug(std::vector<std::string> inputs) {
    communicate("handle debug");
    if (inputs.size() == 2 && inputs[1] == "on") {

    } else if (inputs.size() == 2 && inputs[1] == "off") {

    }
}

void UCI::handleSetOption(std::vector<std::string> inputs) {
    communicate("handle set options");
}

void UCI::handleRegister(std::vector<std::string> inputs) {
    communicate("handle register");
}

void UCI::handleUCI_NewGame() {
    communicate("handle ucinewgame");
}

void UCI::handlePosition(std::vector<std::string> inputs) {
    // Insert dummy command at start to use concatFEN
    inputs.insert(inputs.begin(), "set");
    this->pos.parseFen(concatFEN(inputs));
}

void UCI::handleGo(std::vector<std::string> inputs, std::atomic_bool& stop) {
    communicate("Handling go command. For now, assuming a go infinite command");
    stop = false;
    std::thread searchThread(&Pos::search, &this->pos, this->params, std::ref(stop));
    searchThread.detach();
}

void UCI::handleStop(std::atomic_bool& stop) {
    communicate("handle stop");
    stop = true;
    // Print best move/variation here.
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
        if (commands[0] == "isready") this->handleIsReady();
        else if (commands[0] == "setoption") this->handleSetOption();
        else if (commands[0] == "debug") this->handleDebug(commands);
        else if (commands[0] == "setoption") this->handleSetOption(commands);
        else if (commands[0] == "register") this->handleRegister(commands);
        else if (commands[0] == "ucinewgame") this->handleUCI_NewGame();
        else if (commands[0] == "position") this->handlePosition(commands);
        else if (commands[0] == "go") this->handleGo(commands, stop);
        else if (commands[0] == "stop") this->handleStop(stop);
        else if (commands[0] == "ponderhit") this->handlePonderHit();
        std::cout << std::flush;
        std::getline(std::cin, input);
    }
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
