#ifndef MCTS_HPP
#define MCTS_HPP

#include "search.hpp"

class Mcts : public BaseSearcher {
	public:
		Mcts(Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs);
		void search();
};

class MctsNode : public BaseNode {
	public:
		MctsNode(MctsNode* parent, Move move, Position& pos, SearchInfo& searchInfo, float evaluation);
		MctsNode* bestChild();
		MctsNode* bestAvgValueChild();
		MctsNode* select();
		MctsNode* expand();
		float simulate();
		void rollback(float val);
		const std::vector<MctsNode*> getChildren() const;
		inline float getValue() { return this->value; }
		inline uint getVisits() { return this->visits; }
		float UCT() const;
		float averageValue() const;
		float getEvaluation() const;

	private:
		float value;
		uint visits;
		float evaluation;

		void addChild(Move move);
		uint getVisits() const { return this->visits; }

		struct UCTComp {
			bool operator()(const std::unique_ptr<BaseNode>& a, const std::unique_ptr<BaseNode>& b) const {
				return static_cast<MctsNode*>(a.get())->UCT() < static_cast<MctsNode*>(b.get())->UCT();
			}
		};
};

#endif