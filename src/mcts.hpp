#pragma once

#include <atomic>
#include <chrono>

#include "constants.hpp"
#include "game.hpp"
#include "sicario.hpp"

/**
 * @brief Struct used to store information about the current tree search. Used mainly for sending the UCI info message.
 */
struct SearchInfo {
	public:
		/**
		 * @brief Get the depth (in plies) of the current search.
		 *
		 * @return Depth of the current search in plies.
		 */
		int getDepth() const;

		/**
		 * @brief Set the depth (in plies) of the current search.
		 *
		 * @param Depth Depth of the current search in plies.
		 */
		void setDepth(int depth);

		/**
		 * @brief Get the selective depth (in plies) of the deepest PV line.
		 *
		 * @return Depth of the deepest PV line in plies.
		 */
		int getSeldepth() const;

		/**
		 * @brief Set the selective depth (in plies) of the deepest PV line.
		 *
		 * @param seldepth
		 */
		void setSeldepth(int seldepth);

		/**
		 * @brief Return whether or not to print the next UCI info command. This is currently set to true whenever there
		 * is a change in information in any of the member variables.
		 *
		 * @return True if the next UCI info command should be sent. Else, false.
		 */
		bool getPrintInfo() const;

		/**
		 * @brief Set the printInfo member variable.
		 *
		 * @param printInfo The value to set the printInfo command to.
		 */
		void setPrintInfo(bool printInfo);

		/**
		 * @brief Get the current move (from the root) being searched.
		 *
		 * @return Current move (from the root) being searched.
		 */
		Move getCurrMove() const;

		/**
		 * @brief Set the current move (from the root) being searched.
		 *
		 * @param move The move to set the currMove member variable to.
		 */
		void setCurrMove(Move move);

		/**
		 * @brief Get the number of nodes that have been searched. A node is considered "searched" if it has gone
		 * through the simulation stage at least once.
		 *
		 * @return The number of nodes searched.
		 */
		int getNodes() const;

		/**
		 * @brief Increment the nodes count.
		 */
		void incrementNodes();

	private:
		/**
		 * @brief True, if the engine should print the UCI info command. False, otherwise.
		 */
		bool printInfo = false;

		/**
		 * @brief The depth (in plies) of the current search.
		 */
		int depth = 0;

		/**
		 * @brief Also called selective depth - the depth (in plies) of the deepest PV line.
		 */
		int selDepth = 0;

		/**
		 * @brief The number of nodes searched. A node is considered "searched" if it has gone through the simulation
		 * stage at least once.
		 */
		int nodes = 0;

		/**
		 * @brief Current move (from the root) being searched.
		 */
		Move currMove;
};

class Mcts {
	public:
		Mcts(Position& pos, const std::atomic_bool& searchTree, const Option& options) :
				pos(pos),
				rootPlayer(pos.getTurn()),
				searchTree(searchTree),
				options(options) {}
		void search();
		inline Position& getPos() { return this->pos; }
		bool rootIsEOG();

	private:
		Position pos;
		const Player rootPlayer;
		const std::atomic_bool& searchTree;
		const Option& options;
};

class Node {
	public:
		Node(Node* parent, Move move, Position& pos, SearchInfo& searchInfo, const Option& options) :
				depth(parent == nullptr ? 0 : parent->depth + 1),
				inEdge(move),
				parent(parent),
				searchInfo(searchInfo),
				pos(pos),
				rootPlayer(pos.getOriginalTurn()),
				options(options) {
		}

		Node* select();

		Node* expand();

		ExitCode simulate();

		void rollback(ExitCode code);

		Node* bestChild();

		Node* bestChildPv(int pvLine);

		void rootInitialise();

		inline Move getInEdge() const { return this->inEdge; }

		inline Position& getPos() { return this->pos; }

		const std::vector<Node*> getChildren() const;

		inline float getValue() { return this->value; }

		inline uint getVisits() { return this->visits; }

		float Ucb1() const;

	private:
		float value = 0;
		uint visits = 0;

		int depth;
		Move inEdge;
		Node* parent;
		SearchInfo& searchInfo;
		std::vector<std::unique_ptr<Node>> children; // CHECK unique? or shared?
		Position& pos;
		const Player rootPlayer;

		const Option& options;

		void addChild(Move move);
		uint getVisits() const { return this->visits; }

		struct Ucb1Comp {
			bool operator()(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) const {
				return a.get()->Ucb1() < b.get()->Ucb1();
			}
		};
};