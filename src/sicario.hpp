#pragma once

#include <atomic>
#include <thread>
#include "constants.hpp"
#include "game.hpp"

struct SetOptionPair;

template <typename T>
class OptionConfig {
	public:
		/**
		 * @brief Construct a new OptionConfig object. Used for options of type check (bool) or string.
		 *
		 * @param name Name of the option.
		 * @param type Type fo the option.
		 * @param def The default value of the option.
		 */
		OptionConfig(std::string name, std::string type, T def) :
				name(name),
				type(type),
				def(def) {
			this->value = def;
		}

		/**
		 * @brief Construct a new OptionConfig object. Used for options of type spin (int).
		 *
		 * @param name Name of the option.
		 * @param type Type fo the option.
		 * @param def The default value of the option.
		 * @param min Minimum value the option can take.
		 * @param max Maximum value the option can take.
		 */
		OptionConfig(std::string name, std::string type, T def, T min, T max) :
				name(name),
				type(type),
				def(def),
				min(min),
				max(max) {
			this->value = def;
		}

		/**
		 * @brief Construct a new OptionConfig object. Used for options of type combo (string) with predefined values.
		 *
		 * @param name Name of the option.
		 * @param type Type fo the option.
		 * @param def The default value of the option.
		 * @param vars The possible values the option can take.
		 */
		OptionConfig(std::string name, std::string type, T def, std::vector<T> vars) :
				name(name),
				type(type),
				def(def),
				vars(vars) {
			this->value = def;
		}

		/**
		 * @brief Construct a new OptionConfig object. Used for options of type button.
		 *
		 * @param name Name of the option.
		 * @param type Type fo the option.
		 */
		OptionConfig(std::string name, std::string type) :
				name(name),
				type(type) {}

		/**
		 * @brief Get the minimum value of the option.
		 *
		 * @return String of the minimum value.
		 */
		T getMin() { return this->min; }

		/**
		 * @brief Get the maximum value of the option.
		 *
		 * @return String of the maximum value.
		 */
		T getMax() { return this->max; }

		/**
		 * @brief Get the vector of predefined values for the option.
		 *
		 * @return Reference to the vector of predefined values.
		 */
		std::vector<T>& getVars() { return this->vars; }

		/**
		 * @brief Get the current value of the option.
		 *
		 * @return Current value of the option.
		 */
		T getValue() const { return this->value; }

		/**
		 * @brief Set the value of the option.
		 *
		 * @param value Value to set the option to.
		 */
		void setValue(T value) { this->value = value; }

		/**
		 * @brief Get a string representation of the option for the UCI option command.
		 *
		 * @return UCI string representation of the object.
		 */
		std::string toUciString() const;

		/**
		 * @brief Get a string representation of the option
		 *
		 * @return String representation of the option.
		 */
		std::string toString() const;

	private:
		std::string name;
		std::string type;
		T def;
		T min;
		T max;
		std::vector<T> vars = {};
		T value;
};

struct Option {
	bool debugMode = false; // This is set with the "debug" command, not "setoption".
	bool letterMode = false; // This is set with the "lettermode" command, not "setoption".

	// NOTE Currently, the default, min, max and var are all arbitrary
	OptionConfig<int> ThreadOption {"Thread", "spin", 1, 1, 512};
	OptionConfig<int> HashOption { "Hash", "spin", 16, 0, 5000 };
	OptionConfig<bool> PonderOption { "Ponder", "check", false };
	OptionConfig<bool> OwnBookOption { "OwnBook", "check", false };
	OptionConfig<int> MultiPVOption { "MultiPV", "spin", 1, 1, 5 };
	OptionConfig<bool> UCIShowRefutationsOption { "UCI_ShowRefutations", "check", false };
	OptionConfig<bool> UCILimitStrengthOption { "UCI_LimitStrength", "check", false };
	OptionConfig<int> UCIEloOption { "UCI_Elo", "spin", 3000, 1000, 3500 };
	OptionConfig<bool> UCIAnalyseModeOption { "UCI_AnalyseMode", "check", true };
	OptionConfig<std::string> UCIOpponentOption { "UCI_Opponent", "string", "" };
	OptionConfig<bool> ClearHashOption { "ClearHash", "button" };
	// OptionConfig<std::string> FooOption { "foo", "combo", "foo", {"foo", "bar", "baz", "foobar"} };
};

struct SearchParams {
	std::vector<Move> searchMoves; // Need to check for en passant and castling separately against a Position.
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
		Sicario() {};

		/**
		 * @brief Start and run the engine main loop.
		 */
		void run();

	private:
		Position position;
		Option options;
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
		 * @brief Set an option of type check.
		 *
		 * @param option OptionConfig object for the option.
		 * @param value String of the value to set the option to.
		 */
		void setCheckOption(OptionConfig<bool>& option, const std::string& value);

		/**
		 * @brief Set an option of type spin.
		 *
		 * @param option OptionConfig object for the option.
		 * @param value String of the value to set the option to.
		 */
		void setSpinOption(OptionConfig<int>& option, const std::string& value);

		/**
		 * @brief Set an option of type combo.
		 *
		 * @param option OptionConfig object for the option.
		 * @param value String of the value to set the option to.
		 */
		void setComboOption(OptionConfig<std::string>& option, const std::string& value);

		/**
		 * @brief Set an option of type string.
		 *
		 * @param option OptionConfig object for the option.
		 * @param value String of the value to set the option to.
		 */
		void setStringOption(OptionConfig<std::string>& option, const std::string& value);

		/**
		 * @brief Set the Uci_Opponent option.
		 *
		 * @param option OptionConfig object for the option.
		 * @param value String of the value to set the option to.
		 */
		void setUciOpponentOption(OptionConfig<std::string>& option, const std::string& value);

		/**
		 * @brief Clear the cache.
		 */
		void clearCache();

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