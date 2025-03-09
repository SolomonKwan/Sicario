#pragma once

#include "sicario.hpp"
#include "game.hpp"
#include "mcts.hpp"

namespace Uci {
	/**
	 * Print UCI message to stdout. This method is used to distinguish from other non-UCI messages to stdout.
	 *
	 * @param message: The message to be sent to the UCI GUI.
	 */
	void send(const std::string message);

	/**
	 * @brief Send the UCI "id" commands. Called upon receiving the "uci" command.
	 */
	void sendId();

	/**
	 * @brief Send the UCI "uciok" command.
	 */
	void sendUciok();

	/**
	 * @brief Send the readyOk message.
	 */
	void sendReadyOk();

	/**
	 * @brief Send the bestmove message.
	 *
	 * @param root Pointer to McstNode object that is the root of the search tree.
	 * @param debugMode Flag to display information on all children. Used for development and debugging.
	 */
	void sendBestMove(Node* root, bool debugMode);

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
	 *
	 * @param searchInfo Struct containing the search information.
	 * @param root Pointer to the root node object.
	 * @param options Configurations for the engine.
	 */
	void sendInfo(SearchInfo& searchInfo, Node* root, const Option& options);

	/**
	 * @brief Send the option message.
	 *
	 * @tparam T Data type of the option parameter.
	 * @param option OptionConfig object for the option.
	 */
	template<typename T>
	void sendOption(const OptionConfig<T>& option);
}

namespace Info {
	/**
	 * @brief Return the depth string for the UCI info command. In Sicario, depth is defined as the node depth of the
	 * current search.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @return String of the depth part of the UCI info command.
	 */
	std::string depth(SearchInfo& searchInfo);

	/**
	 * @brief Return the seldepth string for the UCI info command. In Sicario, seldepth is defined as the node depth of
	 * deepest line searched so far.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @return String of the seldepth part of the UCI info command.
	 */
	std::string selDepth(SearchInfo& searchInfo);

	/**
	 * @brief Return the time in milliseconds that the search has been running.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @return String of the time part of the UCI info command.
	 */
	std::string time(SearchInfo& searchInfo);

	/**
	 * @brief Return the number of nodes that have been searched.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @return String of the number of nodes for the UCI info command.
	 */
	std::string nodes(SearchInfo& searchInfo);

	/**
	 * @brief Return a principle variation.
	 *
	 * @param root Root node.
	 * @param pvLine The principle variation ranked as pvLine.
	 * @return String of the princple variation ranked as pvLine.
	 */
	std::string pv(Node* root, int pvLine);

	/**
	 * @brief Return the pv number string.
	 *
	 * @param root Root node.
	 * @param pvLine The princple variation rank.
	 * @return String of the multipv of the UCI info command.
	 */
	std::string multiPv(Node* root, int pvLine);

	/**
	 * @brief Return the score of the current search.
	 *
	 * @param root Root node.
	 * @return String of the score UCI command.
	 */
	std::string score(Node* root);

	/**
	 * @brief Return the centipawn evaluation.
	 *
	 * @param root Root node.
	 * @return String of the UCI centipawn evaluation.
	 */
	std::string cp(Node* root);

	/**
	 * @brief Return the mate if there is one in plies.
	 *
	 * @param root Root node.
	 * @return String of the UCI mate depth in plies.
	 */
	std::string mate(Node* root);

	/**
	 * @brief Return the lowerbound of the evaluation in centipawns.
	 *
	 * @param root Root node.
	 * @return String of the UCI lowerbound centipawn evaluation.
	 */
	std::string lowerBound(Node* root);

	/**
	 * @brief Return the upperbound of the evaluation in centipawns.
	 *
	 * @param root Root node.
	 * @return String of the UCI upperbound centipawn evaluation.
	 */
	std::string upperBound(Node* root);

	/**
	 * @brief Return the current move being searched.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @param root Root node.
	 * @return String of the move being searched for the currmove part of the UCI info command.
	 */
	std::string currMove(SearchInfo& searchInfo, Node* root);

	/**
	 * @brief Return the move number of the current move being searched.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @param root Root node.
	 * @return String of the move number of the current move being searched for the UCI info command.
	 */
	std::string currMoveNumber(SearchInfo& searchInfo, Node* root);

	/**
	 * @brief Return the hashfull value.
	 *
	 * @param searchInfo Object containing the information of the search.
	 * @return String of the hashfull for the UCI info command.
	 */
	std::string hashfull(SearchInfo& searchInfo);

	/**
	 * @brief Return the nodes per seconds of the search.
	 *
	 * @param searchInfo Object containing the nodes per seconds of the search.
	 * @return String of the nodes per seconds for the UCI info command.
	 */
	std::string nps(SearchInfo& searchInfo);
}

/**
 * @brief Show the end of game message.
 *
 * @param code ExitCode of the current position.
 */
void showEogMessage(ExitCode code);

/**
 * @brief Convert an average UCB value to a centipawn value. Currently uses a crude sigmoid function centered at y = 0
 * with asymptotes at (-2500, 2500).
 *
 * @param value The average UCB value.
 * @return A number meant to represent the centipawn value of an average UCB value.
 */
int convertToCentipawn(float value);

struct SetOptionPair {
	std::string name;
	std::string value;
};