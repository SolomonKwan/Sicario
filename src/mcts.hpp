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
	SearchInfo() {}

	public:
		int getDepth() const;
		void setDepth(int depth);

		int getSeldepth() const;
		void setSeldepth(int seldepth);

		bool getPrintInfo() const;
		void setPrintInfo(bool printInfo);

		Move getCurrMove() const;
		void setCurrMove(Move move);

		int getNodes() const;
		void incrementNodes();

		bool sendNextInfo() const;

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
		Node(Node* parent, Move move, Position& pos, SearchInfo& searchInfo) :
				depth(parent == nullptr ? 0 : parent->depth + 1), // TODO check depth
				inEdge(move),
				parent(parent),
				searchInfo(searchInfo),
				pos(pos),
				rootPlayer(pos.getOriginalTurn()) {
			this->searchInfo.setDepth(this->depth); // TODO check depth
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

		void addChild(Move move);
		uint getVisits() const { return this->visits; }

		struct Ucb1Comp {
			bool operator()(const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) const {
				return a.get()->Ucb1() < b.get()->Ucb1();
			}
		};
};