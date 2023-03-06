#include "search.hpp"
#include "sicario.hpp"
#include "mcts.hpp"

#include <iostream>
#include "evaluate.hpp"

void Sicario::search() {
	Mcts searcher(this->getPosition(), this->searchTree, this->sicarioConfigs);
	searcher.search();
}

SearchInfo::SearchInfo()
	: hasChanged(false)
	, depth(0) {}

bool SearchInfo::getHasChanged() {
	return this->hasChanged;
}

void SearchInfo::setHasChanged(bool hasChanged) {
	this->hasChanged = hasChanged;
}

int SearchInfo::getDepth() {
	return this->depth;
}

void SearchInfo::setDepth(int depth) {
	if (this->depth == depth) return;
	this->depth = depth;
	this->setHasChanged(true);
}

BaseSearcher::BaseSearcher(const Position& pos, const std::atomic_bool& searchTree,
		const SicarioConfigs& sicarioConfigs)
	: pos(pos)
	, rootPlayer(pos.getTurn())
	, searchTree(searchTree)
	, sicarioConfigs(sicarioConfigs) {}

Position& BaseSearcher::getPos() {
	return this->pos;
}

BaseNode::BaseNode(BaseNode* parent, Move inEdge, Position& pos, SearchInfo& searchInfo)
	: parent(parent)
	, inEdge(inEdge)
	, pos(pos)
	, rootPlayer(parent == nullptr ? pos.getTurn() : parent->rootPlayer)
	, searchInfo(searchInfo) {
	this->depth = this->parent == nullptr ? 0 : this->parent->depth + 1;
	this->searchInfo.setDepth(std::max(this->searchInfo.getDepth(), this->depth));
}

Move BaseNode::getInEdge() const {
	return this->inEdge;
}

Position& BaseNode::getPos() {
	return this->pos;
}