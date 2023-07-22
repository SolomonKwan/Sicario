#include <memory>
#include <algorithm>
#include <iostream>

#include "sicario.hpp"
#include "mcts.hpp"
#include "uci.hpp"

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

// TODO handle case where the root is a terminal node
void Mcts::search() {
	if (this->rootIsEOG()) return;

	SearchInfo searchInfo = SearchInfo(), oldSearchInfo = SearchInfo();
	std::unique_ptr<MctsNode> root(new MctsNode(nullptr, NULL_MOVE, this->getPos(), searchInfo));

	while (searchTree) {
		oldSearchInfo = searchInfo; // TODO implement smarter way of checking if something has changed

		MctsNode* leaf = root->select();
		leaf = leaf->expand();
		ExitCode code = leaf->simulate();
		leaf->rollback(code);

		if (searchInfo != oldSearchInfo) Uci::sendInfo(searchInfo, root.get());
	}

	Uci::sendInfo(searchInfo, root.get()); // Send final info command.
	Uci::sendBestMove(root.get(), sicarioConfigs.debugMode);
}

MctsNode* MctsNode::bestChild() {
	if (this->children.size() == 0) return nullptr;
	return (*std::max_element(children.begin(), children.end(), MctsNode::Ucb1Comp())).get();
}

MctsNode* MctsNode::select() {
	if (this->children.size() == 0) return this;

	// NOTE this currently just chooses the last one it comes across if there are multiple of equal value
	MctsNode* bestChild = this->bestChild();

	// Set currMove for info command.
	if (this->parent == nullptr)
		searchInfo.currMove = bestChild->getInEdge();

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

	// TODO check if expansion expands into EOG game condition. Need to determine how to handle if this is the case.

	this->getPos().makeMove(this->children[0]->getInEdge());
	this->searchInfo.nodes++;
	return this->children[0].get(); // NOTE currently just getting the first child.
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