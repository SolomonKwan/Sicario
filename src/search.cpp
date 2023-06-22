#include "search.hpp"
#include "sicario.hpp"
#include "mcts.hpp"

void Sicario::search() {
	Mcts searcher(this->getPosition(), this->searchTree, this->sicarioConfigs);
	searcher.search();
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