#pragma once

#include <atomic>
#include <chrono>

#include "constants.hpp"
#include "game.hpp"
#include "sicario.hpp"

struct SearchInfo {
	SearchInfo() {
		this->lastMessage = std::chrono::high_resolution_clock::now();
	}

	public:
		int getDepth() const;
		void setDepth(int depth);

		bool getChanged() const;
		void setChanged(bool changed);

		Move getCurrMove() const;
		void setCurrMove(Move move);

		int getNodes() const;
		void incNodes();

		std::chrono::_V2::system_clock::time_point getStart() const;
		void setLastMessage(std::chrono::_V2::system_clock::time_point time);

		bool sendNextInfo() const;

	private:
		bool changed = false;

		int depth = 0;
		int nodes = 0;
		Move currMove;
		std::vector<std::vector<Move>> pvs;
		std::chrono::_V2::system_clock::time_point lastMessage;
};

class Mcts {
	public:
		Mcts(Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs) :
				pos(pos),
				rootPlayer(pos.getTurn()),
				searchTree(searchTree),
				sicarioConfigs(sicarioConfigs) {}
		void search();
		inline Position& getPos() {
			return this->pos;
		}
		bool rootIsEOG();

	private:
		Position pos;
		const Player rootPlayer;
		const std::atomic_bool& searchTree;
		const SicarioConfigs& sicarioConfigs;
};

class MctsNode {
	public:
		MctsNode(MctsNode* parent, Move move, Position& pos, SearchInfo& searchInfo) :
				depth(parent == nullptr ? 0 : parent->depth + 1),
				inEdge(move),
				parent(parent),
				searchInfo(searchInfo),
				pos(pos),
				rootPlayer(pos.getTurn()) {
			this->searchInfo.setDepth(this->depth);
		}

		MctsNode* bestChild();
		MctsNode* bestChildPv(int pvLine);
		MctsNode* select();
		MctsNode* expand();
		ExitCode simulate();
		void rollback(ExitCode code);

		void rootInitialise();

		inline Move getInEdge() const {
			return this->inEdge;
		}

		inline Position& getPos() {
			return this->pos;
		}

		const std::vector<MctsNode*> getChildren() const;

		inline float getValue() {
			return this->value;
		}

		inline uint getVisits() {
			return this->visits;
		}

		float Ucb1() const;
		void updateMateDepth(int childMateDepth);

		inline int getMateDepth() const {
			return this->mateDepth;
		}

	private:
		float value = 0;
		uint visits = 0;
		int mateDepth = 0;

		int depth;
		Move inEdge;
		MctsNode* parent;
		SearchInfo& searchInfo;
		std::vector<std::unique_ptr<MctsNode>> children; // CHECK unique? or shared?
		Position& pos;
		const Player rootPlayer;

		void addChild(Move move);
		uint getVisits() const { return this->visits; }

		struct Ucb1Comp {
			bool operator()(const std::unique_ptr<MctsNode>& a, const std::unique_ptr<MctsNode>& b) const {
				return a.get()->Ucb1() < b.get()->Ucb1();
			}
		};
};