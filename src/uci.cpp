
#include <iostream>

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

/**
 * Sends the response to the UCI engine after recieving the initial uci command.
 */
void UCI_Instance::sendInitialResponse() {
    communicate("id name " + (std::string) VERSION);
    communicate("id author " + (std::string) AUTHOR);
    // communicate("options");
    communicate("uciok");
}

/**
 * Sends the is readyok.
 */
void UCI_Instance::sendReadyOk() {
    communicate("readyok");
}

void UCI_Instance::sendBestMove() {

}

void UCI_Instance::sendCopyProtection() {

}

void UCI_Instance::sendRegistration() {
    
}

void UCI_Instance::sendInfo() {

}

/************************************** End of engine to interface commands *******************************************/
/**********************************************************************************************************************/



/**********************************************************************************************************************/
/***************************************** Interface to engine commands ***********************************************/

void UCI_Instance::handleIsReady() {
    this->sendReadyOk();
}

void UCI_Instance::handleSetOption() {
    communicate("handle set option");
}

void UCI_Instance::handleDebug(std::vector<std::string> inputs) {
    communicate("handle debug");
    if (inputs.size() == 2 && inputs[1] == "on") {

    } else if (inputs.size() == 2 && inputs[1] == "off") {

    }
}

void UCI_Instance::handleSetOption(std::vector<std::string> inputs) {
    communicate("handle set options");
}

void UCI_Instance::handleRegister(std::vector<std::string> inputs) {
    communicate("handle register");
}

void UCI_Instance::handleUCI_NewGame() {
    communicate("handle ucinewgame");
}

void UCI_Instance::handlePosition(std::vector<std::string> inputs) {
    communicate("handle position");
}

void UCI_Instance::handleGo(std::vector<std::string> inputs) {
    communicate("handle go");
}

void UCI_Instance::handleStop() {
    communicate("handle stop");
}

void UCI_Instance::handlePonderHit() {
    communicate("handle ponderhit");
}

/************************************** End of interface to engine commands *******************************************/
/**********************************************************************************************************************/



/**
 * The main loop that constantly reads from stdin for UCI commands. Calls other functions that create threads which
 * handle different parts of program.
 * @param input: THe initial input string.
 */
void runUCI(std::string input) {
    UCI_Instance uci_game;
    uci_game.sendInitialResponse();
    while (input != "quit" && input != "exit" && input != "q") {
        std::vector<std::string> commands = split(input, " ");
        if (commands[0] == "isready") uci_game.handleIsReady();
        else if (commands[0] == "setoption") uci_game.handleSetOption();
        else if (commands[0] == "debug") uci_game.handleDebug(commands);
        else if (commands[0] == "setoption") uci_game.handleSetOption(commands);
        else if (commands[0] == "register") uci_game.handleRegister(commands);
        else if (commands[0] == "ucinewgame") uci_game.handleUCI_NewGame();
        else if (commands[0] == "position") uci_game.handlePosition(commands);
        else if (commands[0] == "go") uci_game.handleGo(commands);
        else if (commands[0] == "stop") uci_game.handleStop();
        else if (commands[0] == "ponderhit") uci_game.handlePonderHit();
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
    runUCI(input);
}
