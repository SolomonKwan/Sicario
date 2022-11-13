
#ifndef SICARIO_HPP
#define SICARIO_HPP

#include <atomic>
#include "constants.hpp"
#include "game.hpp"
#include "uci.hpp"

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
		/**
		 * @brief Start and run the engine main loop.
		 */
		void run();

		/**
		 * @brief Get a copy of the Position object.
		 *
		 * @return Copy of the Position object.
		 */
		Position getPosition() const;

	private:
		Position position;
		SicarioConfigs sicarioConfigs;
		std::atomic_bool searchTree = false;

		// Configurable options of the engine
		// NOTE Currently, the default, min, max and var are all arbitrary
		OptionInfo thread { "Thread", "spin", "1", "1", "512" };
		OptionInfo hash { "Hash", "spin", "16", "0", "5000" };
		OptionInfo ponder { "Ponder", "check", "false" };
		OptionInfo ownBook { "OwnBook", "check", "false" };
		OptionInfo multiPv { "MultiPV", "spin", "1", "1", "5" };
		OptionInfo uciShowCurrLine { "UCI_ShowCurrLine", "check", "false" };
		OptionInfo uciShowRefutations { "UCI_ShowRefutations", "check", "false" };
		OptionInfo uciLimitStrength { "UCI_LimitStrength", "check", "false" };
		OptionInfo uciElo { "UCI_Elo", "spin", "3000", "1000", "3500" };
		OptionInfo uciAnalyseMode { "UCI_AnalyseMode", "check", "true" };
		OptionInfo uciOpponent { "UCI_Opponent", "string", "" };

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
		 * @brief Handle the perft command. Used for debugging.
		 */
		void handlePerft(const std::vector<std::string>&);

		/**
		 * @brief Handle the move command. Used for debugging.
		 */
		void handleMove(const std::vector<std::string>&);

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
		 */
		void handleRandom();

		/**
		 * @brief Handle the state command. Used for debugging.
		 */
		void handleState();

		/**
		 * @brief Handle the options command. Used for debugging.
		 */
		void handleOptions();

		/**
		 * Print all UCI messages to stdin. Used to distinguish from other print commands.
		 *
		 * @param communication: The communication message to be sent to Uci GUI.
		 */
		void communicate(const std::string communication);

		/**
		 * @brief Send the options that the user can set.
		 *
		 * @param option Struct containing the object information.
		 */
		void sendOption(const OptionInfo& option);

		/**
		 * @brief Send the readyOk message.
		 */
		void sendReadyOk();

		/**
		 * @brief Send the bestmove message.
		 */
		void sendBestMove();

		/**
		 * @brief Send the copyProtection message.
		 */
		void sendCopyProtection();

		/**
		 * @brief Send the registration message.
		 */
		void sendRegistration();

		/**
		 * @brief Send the info message.
		 */
		void sendInfo();

		/**
		 * @brief Send the invalid command message.
		 *
		 * @param commands Entire input given to the GUI.
		 */
		void sendInvalidCommand(const std::vector<std::string>& commands);

		/**
		 * @brief Send the missing argument message.
		 *
		 * @param commands Entire input given to the GUI.
		 */
		void sendMissingArgument(const std::vector<std::string>&);

		/**
		 * @brief Send the invalid argument message.
		 *
		 * @param commands Entire input given to the GUI.
		 */
		void sendInvalidArgument(const std::vector<std::string>&);

		/**
		 * @brief Send the unknown option message.
		 *
		 * @param commands Entire input given to the GUI.
		 */
		void sendUnknownOption(const std::vector<std::string>&);

		/**
		 * @brief Send the invalid value message.
		 *
		 * @param commands Entire input given to the GUI.
		 */
		void sendInvalidValue(const std::vector<std::string>&);

		/**
		 * @brief Send the argument out fo range message.
		 *
		 * @param commands Entire input given to the GUI.
		 */
		void sendArgumentOutOfRange(const std::vector<std::string>&);

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
		 * @brief Get the option name from the setOption command.
		 *
		 * @return Entire input given to the GUI.
		 */
		std::string getOptionName(const std::vector<std::string>&);

		/**
		 * @brief Get the option value from the setOption command.
		 *
		 * @return Entire input given to the GUI.
		 */
		std::string getOptionValue(const std::vector<std::string>&);

		/**
		 * @brief Begins search.
		 */
		void mcts();

		/**
		 * @brief Begin the perft calculation.
		 *
		 * @param depth Depth to calculate to.
		 * @param root True, if we are at the root of the perft call. Defauled to false.
		 * @return Number of positions at the specified depth.
		 */
		uint64_t perft(const int depth, const bool root = false);
};

#endif
