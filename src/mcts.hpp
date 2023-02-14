#ifndef MCTS_HPP
#define MCTS_HPP

#include <iostream>
#include "search.hpp"

class Mcts : public BaseSearcher {
	public:
		Mcts(Position& pos) : BaseSearcher(pos) {}
		void search(const std::atomic_bool& searchTree);
};

class MctsNode : public BaseNode {
	public:
		MctsNode(MctsNode* parent, Move move, Position& pos);
		MctsNode* bestChild();
		MctsNode* select();
		MctsNode* expand();
		float simulate();
		void rollback(float val);

	private:
		float value = 0;
		uint visits = 0;

		void addChild(Move move);
		float Ucb1() const;
		uint getVisits() const { return this->visits; }

		struct Ucb1Comparator {
			bool operator()(const std::unique_ptr<BaseNode>& a, const std::unique_ptr<BaseNode>& b) const {
				return dynamic_cast<MctsNode*>(a.get())->Ucb1() < dynamic_cast<MctsNode*>(b.get())->Ucb1();
			}
		};
};

#endif