#include <memory>
#include <iostream>

#include "sicario.hpp"
#include "mcts.hpp"

// NOTE Due to way that the tree is constructed, it may result in stack overflow error due to node deletion/pruning.

const float C = std::sqrt(2);

void Mcts::search(const std::atomic_bool& searchTree) {
	std::unique_ptr<MctsNode> root(new MctsNode(nullptr, NULL_MOVE, this->getPos()));
	while (searchTree) {
		MctsNode* leaf = root->select();
		leaf = leaf->expand();
		float val = leaf->simulate();
		leaf->rollback(val);
	}
}

MctsNode::MctsNode(MctsNode* parent, Move move, Position& pos) : BaseNode(parent, pos) {
	this->inEdge = move; // CHECK initialise here? or in initialiser list?
}

std::unique_ptr<BaseNode>& MctsNode::bestChild() {
	return *std::max_element(children.begin(), children.end(), MctsNode::Ucb1Comparator());
}

MctsNode* MctsNode::select() {
	if (this->children.size() == 0) return this;
	// NOTE this currently just chooses the last one it comes across if there are multiple of equal value
	std::unique_ptr<BaseNode>& bestChild = this->bestChild();
	this->getPos().processMakeMove(bestChild->getInEdge());
	return dynamic_cast<MctsNode*>(bestChild.get())->select();
}

MctsNode* MctsNode::expand() {
	MoveList moves = MoveList(this->getPos());
	if (visits == 0 || this->getPos().isEOG(moves)) return this;

	for (Move move : moves)
		this->addChild(move);

	this->getPos().processMakeMove(this->children[0]->getInEdge());
	return dynamic_cast<MctsNode*>(this->children[0].get()); // NOTE currently just getting the first child.
}

float MctsNode::simulate() {
	MoveList moves = MoveList(this->pos);
	int moveCount = 0;
	ExitCode code;
	while (!(code = this->pos.isEOG(moves))) {
		this->pos.processMakeMove(moves.randomMove());
		moves = MoveList(this->pos);
		moveCount++;
	}

	while (moveCount > 0) {
		this->pos.processUndoMove();
		moveCount--;
	}

	if (code == WHITE_WINS) {
		return this->rootPlayer == WHITE ? 1 : -1;
	} else if (code == BLACK_WINS) {
		return this->rootPlayer == BLACK ? 1 : -1;
	}
	return 0;
}

void MctsNode::rollback(float val) {
	MctsNode* curr = this;
	while (curr->parent != nullptr) {
		curr->visits++;
		curr->value += this->pos.getTurn() == this->rootPlayer ? -1 * val : val;
		curr = dynamic_cast<MctsNode*>(curr->parent);
		this->pos.processUndoMove();
	}

	curr->value += this->pos.getTurn() == this->rootPlayer ? -1 * val : val;
	curr->visits++;
}

void MctsNode::addChild(Move move) {
	this->children.push_back(std::unique_ptr<MctsNode>(new MctsNode(this, move, this->getPos())));
}

float MctsNode::Ucb1() const {
	if (this->visits == 0) return std::numeric_limits<float>::max();
	return (value / static_cast<float>(visits)) + C *
			std::sqrt(std::log(static_cast<float>(dynamic_cast<MctsNode*>(this->parent)->getVisits())) /
			static_cast<float>(visits));
}