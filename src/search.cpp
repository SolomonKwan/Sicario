#include "search.hpp"
#include "sicario.hpp"
#include "mcts.hpp"

#include <iostream>
#include "evaluate.hpp"

void Sicario::search() {
	Mcts searcher(this->getPosition());
	std::cout << Evaluator::psqtEvaluation(this->getPosition()) << '\n';
	searcher.search(this->searchTree, this->sicarioConfigs);
}

Position& BaseSearcher::getPos() {
	return this->pos;
}

Move BaseNode::getInEdge() const {
	return this->inEdge;
}

Position& BaseNode::getPos() {
	return this->pos;
}