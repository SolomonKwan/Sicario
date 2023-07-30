#pragma once

#include "sicario.hpp"
#include "game.hpp"
#include "mcts.hpp"

namespace Uci {
	/**
	 * Print all UCI messages to stdout. Used to distinguish from other print commands.
	 *
	 * @param communication: The communication message to be sent to Uci GUI.
	 */
	void communicate(const std::string communication);

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
	void sendBestMove(MctsNode* root, bool debugMode);

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
	 * @param sicarioConfigs Configurations for the engine.
	 */
	void sendInfo(SearchInfo& searchInfo, MctsNode* root, const SicarioConfigs& sicarioConfigs);

	/**
	 * @brief Send the options that the user can set.
	 *
	 * @param option Struct containing the object information.
	 */
	void sendOption(const OptionInfo& option);
}

namespace Info {
	void send(std::string string, char end = ' ');
	void info();
	void depth(SearchInfo& searchInfo);
	void nodes(SearchInfo& searchInfo);
	void pv(MctsNode* root, int pvLine);
	void multiPv(MctsNode* root, int pvLine);
	void score(MctsNode* root);
	void currMove(SearchInfo& searchInfo, MctsNode* root);
	void nps(SearchInfo& searchInfo);
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