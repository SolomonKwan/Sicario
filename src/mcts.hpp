#pragma once

#include <atomic>
#include <chrono>

#include "constants.hpp"
#include "game.hpp"
#include "sicario.hpp"

struct SearchInfo {
	int depth = 0;
	int nodes = 0;
	Move currMove;
	std::chrono::_V2::system_clock::time_point start;

	SearchInfo() {
		this->start = std::chrono::high_resolution_clock::now();
	}

	bool operator!=(const SearchInfo& s) {
		bool depthChange = this->depth != s.depth;
		return depthChange;
	}
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

	private:
		Position pos;
		const Player rootPlayer;
		const std::atomic_bool& searchTree;
		const SicarioConfigs& sicarioConfigs;
};

class MctsNode {
	public:
		MctsNode(MctsNode* parent, Move move, Position& pos, SearchInfo& searchInfo) :
				value(0),
				visits(0),
				mateDepth(0),
				depth(parent == nullptr ? 0 : parent->depth + 1),
				inEdge(move),
				parent(parent),
				searchInfo(searchInfo),
				pos(pos),
				rootPlayer(pos.getTurn()) {
			this->searchInfo.depth = std::max(this->searchInfo.depth, this->depth);
		}

		MctsNode* bestChild();
		MctsNode* select();
		MctsNode* expand();
		ExitCode simulate();
		void rollback(ExitCode code);

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
		float value;
		uint visits;
		int mateDepth;

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