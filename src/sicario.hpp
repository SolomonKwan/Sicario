#pragma once

#include <atomic>
#include <thread>
#include "constants.hpp"
#include "game.hpp"

struct SetOptionPair;

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
	bool letterMode = false; // This is set with the "lettermode" command, not "setoption".
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
		 * @brief Handle the data command. Used for debugging.
		 */
		void handleData();

		/**
		 * @brief Handle the has command. Used for debugging.
		 */
		void handleHash();

		/**
		 * @brief Handle the lettermode command.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void handleLetterMode(const std::vector<std::string>& inputs);

		/**
		 * @brief Send the invalid command message.
		 *
		 * @param inputs Entire input given to the GUI.
		 * @param customMsg A custom error message.
		 */
		void sendInvalidCommand(const std::vector<std::string>& inputs, const std::string& customMsg = "");

		/**
		 * @brief Send the missing argument message.
		 *
		 * @param inputs Entire input given to the GUI.
		 * @param customMsg A custom error message.
		 */
		void sendMissingArgument(const std::vector<std::string>& inputs, const std::string& customMsg = "");

		/**
		 * @brief Send the invalid argument message.
		 *
		 * @param value The invalid value.
		 * @param customMsg A custom error message.
		 */
		void sendInvalidArgument(const std::string& value, const std::string& customMsg = "");

		/**
		 * @brief Send the unknown option message.
		 *
		 * @param option The unknown option.
		 * @param customMsg A custom error message.
		 */
		void sendUnknownOption(const std::string& option, const std::string& customMsg = "");

		/**
		 * @brief Send the invalid value message.
		 *
		 * @param value The value that is invalid.
		 * @param customMsg A custom error message.
		 */
		void sendInvalidValue(const std::string& value, const std::string& customMsg = "");

		/**
		 * @brief Send the argument out of range message.
		 *
		 * @param value The value that is out of range.
		 * @param customMsg A custom error message.
		 */
		void sendArgumentOutOfRange(const std::string& value, const std::string& customMsg = "");

		/**
		 * @brief Set the Thread option.
		 *
		 * @param value The value to set Thread to.
		 */
		void setOptionThread(const std::string& value);

		/**
		 * @brief Set the Hash option.
		 *
		 * @param inputs The value to set Hash to.
		 */
		void setOptionHash(const std::string& value);

		/**
		 * @brief Set the Ponder option.
		 *
		 * @param value The value to set Ponder to.
		 */
		void setOptionPonder(const std::string& value);

		/**
		 * @brief Set the OwnBook option.
		 *
		 * @param value The value to set OwnBook to.
		 */
		void setOptionOwnBook(const std::string& value);

		/**
		 * @brief Set the MultiPV option.
		 *
		 * @param value The value to set MultiPV to.
		 */
		void setOptionMultiPV(const std::string& value);

		/**
		 * @brief Set the UCI_ShowCurrLine option.
		 *
		 * @param value The value to set UCI_ShowCurrLine to.
		 */
		void setOptionUciShowCurrLine(const std::string& value);

		/**
		 * @brief Set the UCI_ShowRefutations option.
		 *
		 * @param value The value to set UCI_ShowRefutations to.
		 */
		void setOptionUciShowRefutations(const std::string& value);

		/**
		 * @brief Set the UCI_LimitStrength option.
		 *
		 * @param value The value to set UCI_LimitStrength to.
		 */
		void setOptionUciLimitStrength(const std::string& value);

		/**
		 * @brief Set the UCI_Elo option.
		 *
		 * @param value The value to set UCI_Elo to.
		 */
		void setOptionUciElo(const std::string& value);

		/**
		 * @brief Set the UCI_AnalyseMode option.
		 *
		 * @param value The value to set UCI_AnalyseMode to.
		 */
		void setOptionUciAnalyseMode(const std::string& value);

		/**
		 * @brief Set the UCI_Opponent option.
		 *
		 * @param value The value to set UCI_Opponent to.
		 */
		void setOptionUciOpponent(const std::string& value);

		/**
		 * @brief Clear the hash.
		 */
		void setOptionClearHash();

		/**
		 * @brief Get the name and value of the setoption command.
		 *
		 * @param inputs Entire input given to the GUI.
		 * @return SetOptionPair struct containing the name and value.
		 */
		SetOptionPair getOptionNameAndValue(const std::vector<std::string>& inputs);

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