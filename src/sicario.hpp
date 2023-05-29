#pragma once

#include <atomic>
#include <thread>
#include "constants.hpp"
#include "game.hpp"

struct OptionInfo {
	std::string name = "";
	std::string type = "";
	std::string def = "";
	std::string min = "";
	std::string max = "";
	std::vector<std::string> vars = {};
	std::string value;
};

struct SicarioConfigs {
	bool debugMode = false; // This is set with the "debug" command, not "setoption".
	OptionInfo options[CONFIGS_COUNT];
};

/**
 * @brief Show the engine information.
 *
 * @param argc Number of command line arguments
 * @param argv Command line arguments.
 */
void showInfo(int argc, char* argv[]);

/**
 * @brief Show the logo.
 */
void showLogo();

/**
 * @brief Show startup information.
 */
void showStartUp();

class Sicario {
	public:
		Sicario();

		/**
		 * @brief Start and run the engine main loop.
		 */
		void run();

	private:
		Position position;
		SicarioConfigs sicarioConfigs;
		std::vector<std::thread> threads;
		std::atomic_bool searchTree = false;

		/**
		 * Parses the GUI input string and calls handlers for commands.
		 *
		 * @param input Input string from GUI.
		 */
		void processInput(const std::string& input);

		/**
		 * @brief Hash the input string to the GUI.
		 *
		 * @return Type of the input string.
		 */
		UciInput hashCommandInput(const std::string&);

		/**
		 * @brief Hash the input for a setoption command.
		 *
		 * @param inputs Entire input given to the GUI.
		 * @return Type of the setOption command.
		 */
		ConfigOption hashOptionsInput(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the uci command.
		 */
		void handleUci();

		/**
		 * @brief Handle the isReady command.
		 */
		void handleIsReady();

		/**
		 * @brief Handle the debug command.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handleDebug(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the debug command.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handleSetOption(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the ucinewgame command.
		 */
		void handleUciNewGame();

		/**
		 * @brief Handle the position command.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handlePosition(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the go command.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handleGo(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the stop command.
		 */
		void handleStop();

		/**
		 * @brief Handle the ponderhit command.
		 */
		void handlePonderHit();

		/**
		 * @brief Handle the quit command.
		 */
		void handleQuit();

		/**
		 * @brief Handle the perft command. Used for debugging.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handlePerft(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the move command. Used for debugging.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handleMove(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the undo command. Used for debugging.
		 */
		void handleUndo();

		/**
		 * @brief Handle the display command. Used for debugging.
		 */
		void handleDisplay();

		/**
		 * @brief Handle the moves command. Used for debugging.
		 */
		void handleMoves();

		/**
		 * @brief Handle the bitboards command. Used for debugging.
		 */
		void handleBitboards();

		/**
		 * @brief Handle the random command. Used for debugging.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handleRandom(const std::vector<std::string>& inputs);

		/**
		 * @brief Handle the state command. Used for debugging.
		 */
		void handleState();

		/**
		 * @brief Handle the options command. Used for debugging.
		 */
		void handleOptions();

		/**
		 * @brief Send the invalid command message.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void sendInvalidCommand(const std::vector<std::string>& inputs);

		/**
		 * @brief Send the missing argument message.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void sendMissingArgument(const std::vector<std::string>& inputs);

		/**
		 * @brief Send the invalid argument message.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void sendInvalidArgument(const std::vector<std::string>& inputs);

		/**
		 * @brief Send the unknown option message.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void sendUnknownOption(const std::vector<std::string>& inputs);

		/**
		 * @brief Send the invalid value message.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void sendInvalidValue(const std::vector<std::string>& inputs);

		/**
		 * @brief Send the argument out fo range message.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void sendArgumentOutOfRange(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the thread option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionThread(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the hash option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionHash(const std::vector<std::string>& inputs);

		/**
		 * @brief Clear the hash.
		 */
		void setOptionClearHash();

		/**
		 * @brief Set the ponder option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionPonder(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the ownBook option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionOwnBook(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the multiPV option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionMultiPV(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the uciShowCurrLine option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionUciShowCurrLine(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the uciShowRefutations option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionUciShowRefutations(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the uciLimitStrength option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionUciLimitStrength(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the uciElo option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionUciElo(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the uciAnalyseMode option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionUciAnalyseMode(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the uciOpponent option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionUciOpponent(const std::vector<std::string>& inputs);

		/**
		 * @brief Set the expandTime option.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void setOptionExpandTime(const std::vector<std::string>& inputs);

		/**
		 * @brief Get the option name from the setOption command.
		 *
		 * @param inputs Entire input given to the GUI.
		 * @return Name of the input option.
		 */
		std::string getOptionName(const std::vector<std::string>& inputs);

		/**
		 * @brief Get the option value from the setOption command.
		 *
		 * @param inputs Entire input given to the GUI.
		 * @return Value of the input option.
		 */
		std::string getOptionValue(const std::vector<std::string>& inputs);

		/**
		 * @brief Begins search.
		 */
		void search();

		/**
		 * @brief Begin the perft calculation.
		 *
		 * @param depth Depth to calculate to.
		 * @param root True, if we are at the root of the perft call. Defauled to false.
		 * @return Number of positions at the specified depth.
		 */
		uint64_t perft(const int depth, const bool root = false);

		/**
		 * @brief Get a reference to the Position object.
		 *
		 * @return A reference to the position object.
		 */
		inline Position& getPosition() {
			return this->position;
		}
};