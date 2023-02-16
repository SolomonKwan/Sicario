
#ifndef UCI_HPP
#define UCI_HPP

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
	 */
	void sendInfo(SearchInfo& searchInfo);

	/**
	 * @brief Send the options that the user can set.
	 *
	 * @param option Struct containing the object information.
	 */
	void sendOption(const OptionInfo& option);
}

/**
 * @brief Show the end of game message.
 *
 * @param code ExitCode of the current position.
 */
void showEogMessage(ExitCode code);

#endif
