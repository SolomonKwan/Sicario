#include <memory>

#include "sicario.hpp"
#include "mcts.hpp"
#include "uci.hpp"

// NOTE Due to way that the tree is constructed, it may result in stack overflow error due to node deletion/pruning.

const float C = std::sqrt(2);

void Mcts::search() {
	SearchInfo searchInfo;
	std::unique_ptr<MctsNode> root(new MctsNode(nullptr, NULL_MOVE, this->getPos(), searchInfo));
	while (searchTree) {
		SearchInfo oldSearchInfo = searchInfo; // TODO implement smarter way of checking if something has changed

		MctsNode* leaf = root->select();
		leaf = leaf->expand();
		float val = leaf->simulate();
		leaf->rollback(val);

		if (searchInfo != oldSearchInfo) Uci::sendInfo(searchInfo);
	}
	Uci::sendInfo(searchInfo); // Send final info command.
	Uci::sendBestMove(root.get(), sicarioConfigs.debugMode);
}

MctsNode::MctsNode(MctsNode* parent, Move move, Position& pos, SearchInfo& searchInfo) :
		BaseNode(parent, pos, searchInfo) {
	this->inEdge = move; // CHECK initialise here? or in initialiser list?
	this->depth = parent == nullptr ? 0 : parent->depth + 1;
	this->searchInfo.depth = std::max(this->searchInfo.depth, this->depth);
}

MctsNode* MctsNode::bestChild() {
	if (this->children.size() == 0) return nullptr;
	return dynamic_cast<MctsNode*>((*std::max_element(children.begin(), children.end(), MctsNode::Ucb1Comp())).get());
}

MctsNode* MctsNode::select() {
	if (this->children.size() == 0) return this;
	// NOTE this currently just chooses the last one it comes across if there are multiple of equal value
	MctsNode* bestChild = this->bestChild();
	this->getPos().makeMove(bestChild->getInEdge());
	return bestChild->select();
}

MctsNode* MctsNode::expand() {
	MoveList moves = MoveList(this->getPos());
	if (visits == 0 || this->getPos().isEOG(moves)) return this;

	for (Move move : moves)
		this->addChild(move);

	// TODO check if expansion expands into EOG game condition. Need to determine how to handle if this is the case.

	this->getPos().makeMove(this->children[0]->getInEdge());
	return dynamic_cast<MctsNode*>(this->children[0].get()); // NOTE currently just getting the first child.
}

float MctsNode::simulate() {
	MoveList moves = MoveList(this->pos);
	int moveCount = 0;
	ExitCode code;
	while (!(code = this->pos.isEOG(moves))) {
		this->pos.makeMove(moves.randomMove());
		moves = MoveList(this->pos);
		moveCount++;
	}

	while (moveCount > 0) {
		this->pos.undoMove();
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
		this->pos.undoMove();
	}

	curr->value += this->pos.getTurn() == this->rootPlayer ? -1 * val : val;
	curr->visits++;
}

const std::vector<MctsNode*> MctsNode::getChildren() const {
	std::vector<MctsNode*> children;
	for (auto& child : this->children)
		children.push_back(dynamic_cast<MctsNode*>(child.get()));
	return children;
}

float MctsNode::Ucb1() const {
	if (this->visits == 0) return std::numeric_limits<float>::max();
	return (value / static_cast<float>(visits)) + C *
			std::sqrt(std::log(static_cast<float>(dynamic_cast<MctsNode*>(this->parent)->getVisits())) /
			static_cast<float>(visits));
}

void MctsNode::addChild(Move move) {
	this->children.push_back(std::unique_ptr<MctsNode>(new MctsNode(this, move, this->getPos(), this->searchInfo)));
}