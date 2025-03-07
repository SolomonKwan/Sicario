#pragma once

#include <atomic>
#include <thread>
#include "constants.hpp"
#include "game.hpp"

struct SetOptionPair;

struct OptionInfo {
	// TODO have these set as the types that they are meant to be. e.g., have a button type be set as a bool instead of a string
	std::string name = "";
	std::string type = "";
	std::string def = "";
	std::string min = "";
	std::string max = "";
	std::vector<std::string> vars = {};

	std::string value; // The actual value that has been set for this option.
};

struct SicarioConfigs {
	bool debugMode = false; // This is set with the "debug" command, not "setoption".
	bool letterMode = false; // This is set with the "lettermode" command, not "setoption".
	OptionInfo options[CONFIGS_COUNT];
};

struct SearchParams {
	std::vector<Move> searchMoves; // Need to check for en passant and castling separately against a Position. TODO could do a search on parsing and only add if valid.
	bool ponder = false;
	int wtime = -1;
	int btime = -1;
	int winc = -1;
	int binc = -1;
	int movesToGo = -1;
	int depth = -1;
	int nodes = -1;
	int mate = -1;
	int moveTime = -1;
	bool infinite = false;
	bool suddenDeath = false;

	void reset();
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
		SearchParams searchParams;
		std::vector<std::thread> threads;
		std::atomic_bool searchTree = false;

		/**
		 * Parses the GUI input string and calls handlers for commands.
		 *
		 * @param input Input string from GUI.
		 */
		void processInput(const std::string& input);

		/**
		 * @brief Hash the input string from the GUI.
		 *
		 * @param input Input from the GUI.
		 * @return Type of the input string.
		 */
		UciInput hashCommandInput(const std::string& input);

		/**
		 * @brief Parse the parameters of the search.
		 *
		 * @param inputs Entire input given to the GUI.
		 */
		void parseGo(const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the searchmoves sub-command of the go command.
		 *
		 * @param index Index of the searchmoves sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseSearchmoves(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the ponder sub-command of the go command.
		 *
		 * @param index Index of the ponder sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parsePonder(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the wtime sub-command of the go command.
		 *
		 * @param index Index of the wtime sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseWtime(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the btime sub-command of the go command.
		 *
		 * @param index Index of the btime sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseBtime(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the winc sub-command of the go command.
		 *
		 * @param index Index of the winc sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseWinc(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the binc sub-command of the go command.
		 *
		 * @param index Index of the binc sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseBinc(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the movestogo sub-command of the go command.
		 *
		 * @param index Index of the movestogo sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseMovestogo(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the depth sub-command of the go command.
		 *
		 * @param index Index of the depth sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseDepth(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the nodes sub-command of the go command.
		 *
		 * @param index Index of the nodes sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseNodes(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the mate sub-command of the go command.
		 *
		 * @param index Index of the mate sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseMate(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the movetime sub-command of the go command.
		 *
		 * @param index Index of the movetime sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseMovetime(int index, const std::vector<std::string>& inputs);

		/**
		 * @brief Parse the infinite sub-command of the go command.
		 *
		 * @param index Index of the infinite sub-command in the inputs vector.
		 * @param inputs Vector of inputs from the GUI.
		 */
		void parseInfinite(int index, const std::vector<std::string>& inputs);

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