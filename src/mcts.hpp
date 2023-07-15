#pragma once

#include "search.hpp"

class Mcts : public BaseSearcher {
	public:
		Mcts(Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs) :
				BaseSearcher(pos, searchTree, sicarioConfigs) {}
		void search();
};

class MctsNode : public BaseNode {
	public:
		MctsNode(MctsNode* parent, Move move, Position& pos, SearchInfo& searchInfo);
		MctsNode* bestChild();
		MctsNode* select();
		MctsNode* expand();
		ExitCode simulate();
		void rollback(ExitCode code);
		const std::vector<MctsNode*> getChildren() const;
		inline float getValue() { return this->value; }
		inline uint getVisits() { return this->visits; }
		float Ucb1() const;
		void updateMateDepth(int childMateDepth);
		inline int getMateDepth() const {
			return this->mateDepth;
		}

	private:
		float value = 0;
		uint visits = 0;
		int mateDepth = 0;

		void addChild(Move move);
		uint getVisits() const { return this->visits; }

		struct Ucb1Comp {
			bool operator()(const std::unique_ptr<BaseNode>& a, const std::unique_ptr<BaseNode>& b) const {
				return dynamic_cast<MctsNode*>(a.get())->Ucb1() < dynamic_cast<MctsNode*>(b.get())->Ucb1();
			}
		};
};