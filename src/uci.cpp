
#include <iostream>

#include "game.hpp"
#include "uci.hpp"

/******************************************************************************/
/*********************** Engine to interface commands *************************/

/**
 * Prints all messages to UCI communication to stdin. Used to distinguish from
 * other print commands.
 * @param communication: The communication message to be sent to UCI GUI.
 */
void communicate(std::string communication) {
    std::cout << communication << '\n' << std::flush;
}

/**
 * Sends the response to the UCI engine after recieving the initial uci command.
 * @param uci: Boolean indicating whether the uci command was recieved.
 */
void UCI_InitialResponse() {
    // communicate("id name " + (std::string) VERSION);
    // communicate("id author " + (std::string) AUTHOR);
    // communicate("options"); // To determine.
    // communicate("uciok");
}

/**
 * Sends the is readyok.
 */
void UCI_ReadyOk() {
    communicate("readyok");
}

/******************** End of engine to interface commands *********************/
/******************************************************************************/

/**
 * The main loop that constantly reads from stdin for UCI commands. Calls other
 * functions that create threads which handle different parts of program.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param args: Pointer to command line arguments.
 * @param argv: Command line arguments.
 */
void runUCI(Pos* game, char *argv[]) {
    std::string input;
    UCI_InitialResponse();
    while (std::getline(std::cin, input)) {
        if (input == "quit") break;
    }
}

/**
 * Initialisation to start reading from stdin. Main function that starts
 * main thread of the program and handles all UCI interations.
 * @param game: Pointer to game struct.
 * @param moves: Pointer to precomputed moves struct.
 * @param args: Pointer to command line arguments.
 * @param argv: Command line arguments.
 */
// void UCI::init(Pos* game, char *argv[], std::string input) {
//     runUCI(game, argv);
// }
