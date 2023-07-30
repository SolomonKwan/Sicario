#include <memory>
#include <algorithm>
#include <iostream>
#include <map>

#include "sicario.hpp"
#include "mcts.hpp"
#include "uci.hpp"

int SearchInfo::getDepth() const {
	return this->depth;
}

void SearchInfo::setDepth(int depth) {
	if (this->depth == std::max(depth, this->depth)) return;
	this->changed = true;
	this->depth = std::max(depth, this->depth);
}

void SearchInfo::setChanged(bool changed) {
	this->changed = changed;
}

bool SearchInfo::getChanged() const {
	return this->changed;
}

Move SearchInfo::getCurrMove() const {
	return this->currMove;
}

void SearchInfo::setCurrMove(Move move) {
	this->currMove = move;
}

int SearchInfo::getNodes() const {
	return this->nodes;
}

void SearchInfo::incNodes() {
	this->nodes++;
}

std::chrono::_V2::system_clock::time_point SearchInfo::getStart() const {
	return this->lastMessage;
}

void SearchInfo::setLastMessage(std::chrono::_V2::system_clock::time_point time) {
	this->lastMessage = time;
}

bool SearchInfo::sendNextInfo() const {
	auto now = std::chrono::high_resolution_clock::now();
	int timeSinceLastInfo = std::chrono::duration_cast<std::chrono::seconds>(now - this->getStart()).count();
	return changed || (timeSinceLastInfo >= 3);
}

void Sicario::search() {
	Mcts searcher(this->getPosition(), this->searchTree, this->sicarioConfigs);
	searcher.search();
	this->searchTree = false;
}

bool Mcts::rootIsEOG() {
	MoveList moves = MoveList(this->pos);
	if (this->pos.isCheckmate(moves)) {
		std::cout << "info depth 0 score mate 0" << '\n';
		std::cout << "bestmove (none)" << '\n';
		return true;
	} else if (this->pos.isDrawStalemate(moves)) {
		std::cout << "info depth 0 score cp 0" << '\n';
		std::cout << "bestmove (none)" << '\n';
		return true;
	}
	return false;
}

// NOTE Due to way that the tree is constructed, it may result in stack overflow error due to node deletion/pruning.

void Mcts::search() {
	if (this->rootIsEOG()) return;

	SearchInfo searchInfo = SearchInfo();
	std::unique_ptr<MctsNode> root(new MctsNode(nullptr, NULL_MOVE, this->getPos(), searchInfo));
	root->rootInitialise();

	while (searchTree) {
		MctsNode* leaf = root->select();
		leaf = leaf->expand();
		ExitCode code = leaf->simulate();
		leaf->rollback(code);

		if (searchInfo.sendNextInfo()) Uci::sendInfo(searchInfo, root.get(), this->sicarioConfigs);
	}

	Uci::sendInfo(searchInfo, root.get(), this->sicarioConfigs); // Send final info command.
	Uci::sendBestMove(root.get(), sicarioConfigs.debugMode);
}

MctsNode* MctsNode::bestChild() {
	if (this->children.size() == 0) return nullptr;
	std::vector<MctsNode*> ptrs;
	for (auto& child : this->children) {
		if (ptrs.size() == 0 || child.get()->Ucb1() == ptrs.front()->Ucb1()) {
			ptrs.push_back(child.get());
		} else if (child.get()->Ucb1() > ptrs.front()->Ucb1()) {
			ptrs.clear();
			ptrs.push_back(child.get());
		}
	}
	return ptrs[randInt() % ptrs.size()];
}

MctsNode* MctsNode::bestChildPv(int pvLine) {
	std::map<float, std::vector<MctsNode*>> map;
	for (auto& child : this->children)
		map[child->Ucb1()].push_back(child.get());

	MctsNode* node = nullptr;
	for (auto itr = map.rbegin(); itr != map.rend(); itr++) {
		if (pvLine <= static_cast<int>(itr->second.size())) {
			node = itr->second[pvLine - 1];
			break;
		} else {
			pvLine -= itr->second.size();
		}
	}

	assert(node != nullptr);
	return node;
}

MctsNode* MctsNode::select() {
	if (this->children.size() == 0) return this;

	// Set currMove for info command.
	MctsNode* bestChild = this->bestChild();
	if (this->parent == nullptr)
		searchInfo.setCurrMove(bestChild->getInEdge());

	this->getPos().makeMove(bestChild->getInEdge());
	return bestChild->select();
}

MctsNode* MctsNode::expand() {
	MoveList moves = MoveList(this->getPos());
	if (this->getPos().isCheckmate(moves)) {
		this->mateDepth = -1 * this->depth;
		return this;
	}
	if (visits == 0 || this->getPos().isDrawStalemate(moves)) return this;

	for (Move move : moves)
		this->addChild(move);

	this->getPos().makeMove(this->children[0]->getInEdge());
	this->searchInfo.incNodes();
	return this->children[randInt() % this->children.size()].get();
}

ExitCode MctsNode::simulate() {
	MoveList moves = MoveList(this->pos);
	int moveCount = 0;
	ExitCode code;
	while (!(code = this->pos.isEOG(moves))) {
		this->pos.makeMove(moves.randomMove());
		moves = MoveList(this->pos);
		moveCount++;
	}

	for (; moveCount > 0; moveCount--)
		this->pos.undoMove();

	return code;
}

void MctsNode::rollback(ExitCode code) {
	MctsNode* curr = this;
	while (curr->parent != nullptr) {
		curr->visits++;
		if (code == WHITE_WINS && this->rootPlayer == WHITE) {
			curr->value += 1;
		} else if (code == BLACK_WINS && this->rootPlayer == BLACK) {
			curr->value += 1;
		} else if (code != WHITE_WINS && code != BLACK_WINS) {
			curr->value += 0.5;
		}
		curr->parent->updateMateDepth(-1 * curr->mateDepth);
		curr = curr->parent;
		this->pos.undoMove();
	}
	curr->visits++;
}

void MctsNode::rootInitialise() {
	assert(this->parent == nullptr);
	MoveList moves = MoveList(this->getPos());
	for (Move move : moves)
		this->addChild(move);
}

const std::vector<MctsNode*> MctsNode::getChildren() const {
	std::vector<MctsNode*> children;
	for (auto& child : this->children)
		children.push_back(child.get());
	return children;
}

float MctsNode::Ucb1() const {
	if (this->visits == 0) return std::numeric_limits<float>::max();
	return (value / static_cast<float>(visits)) + std::sqrt(2) *
			std::sqrt(std::log(static_cast<float>(this->parent->getVisits())) /
			static_cast<float>(visits));
}

void MctsNode::addChild(Move move) {
	this->children.push_back(std::unique_ptr<MctsNode>(new MctsNode(this, move, this->getPos(), this->searchInfo)));
}

void MctsNode::updateMateDepth(int childMateDepth) {
	int parent = this->mateDepth;
	int child = childMateDepth;

	if (parent > 0 && child > 0) {
		this->mateDepth = std::min(parent, child);
	} else if (parent < 0 && child > 0) {
		this->mateDepth = child;
	} else if (parent < 0 && child < 0) {
		this->mateDepth = std::max(parent, child);
	} else if (parent < 0 && child == 0) {
		this->mateDepth = 0;
	} else if (parent == 0 && child > 0) {
		this->mateDepth = child;
	}
}