#include <memory>
#include <random>

#include "sicario.hpp"
#include "mcts.hpp"
#include "uci.hpp"

Mcts::Mcts(Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs)
	: BaseSearcher(pos, searchTree, sicarioConfigs) {}

void Mcts::search() {
	SearchInfo searchInfo;
	std::unique_ptr<MctsNode> root(new MctsNode(nullptr, NULL_MOVE, this->getPos(), searchInfo));
	while (searchTree) {
		searchInfo.setHasChanged(false);

		MctsNode* leaf = root->select();
		leaf = leaf->expand();
		float val = leaf->simulate();
		leaf->rollback(val);

		if (searchInfo.getHasChanged()) Uci::sendInfo(searchInfo);
	}
	Uci::sendBestMove(root.get(), sicarioConfigs.debugMode);
}

MctsNode::MctsNode(MctsNode* parent, Move move, Position& pos, SearchInfo& searchInfo)
	: BaseNode(parent, move, pos, searchInfo)
	, value(0)
	, visits(0) {}

MctsNode* MctsNode::bestChild() {
	std::vector<MctsNode*> bestChildren;
	float maxUCT = -std::numeric_limits<float>::max();
	for (auto& child : this->children) {
		float uct = dynamic_cast<MctsNode*>(child.get())->UCT();
		if (uct > maxUCT) {
			bestChildren.clear();
			bestChildren.push_back(dynamic_cast<MctsNode*>(child.get()));
			maxUCT = uct;
		} else if (uct == maxUCT) {
			bestChildren.push_back(dynamic_cast<MctsNode*>(child.get()));
		}
	}

	assert(bestChildren.size() != 0);

	if (bestChildren.size() == 1) return bestChildren.front();
	static std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<std::size_t> distribution(0, bestChildren.size() - 1);
	return bestChildren[distribution(generator)];
}

MctsNode* MctsNode::select() {
	if (this->children.size() == 0) return this;
	MctsNode* bestChild = this->bestChild();
	this->getPos().processMakeMove(bestChild->getInEdge());
	return bestChild->select();
}

MctsNode* MctsNode::expand() {
	MoveList moves = MoveList(this->getPos());
	if (visits == 0 || this->getPos().isEOG(moves)) return this;

	for (Move move : moves)
		this->addChild(move);

	auto bestChild = this->bestChild();
	this->getPos().processMakeMove(bestChild->getInEdge());
	return bestChild;
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
		return this->rootPlayer == WHITE ? 1 : 0;
	} else if (code == BLACK_WINS) {
		return this->rootPlayer == BLACK ? 1 : 0;
	}
	return 0.5;
}

void MctsNode::rollback(float val) {
	MctsNode* curr = this;
	while (curr->parent != nullptr) {
		curr->visits++;
		curr->value += val;
		curr = dynamic_cast<MctsNode*>(curr->parent);
		this->pos.processUndoMove();
	}

	curr->value += val;
	curr->visits++;
}

const std::vector<MctsNode*> MctsNode::getChildren() const {
	std::vector<MctsNode*> children;
	for (auto& child : this->children)
		children.push_back(dynamic_cast<MctsNode*>(child.get()));
	return children;
}

float MctsNode::UCT() const {
	if (this->visits == 0) return std::numeric_limits<float>::max();
	float C = std::sqrt(2);
	float exploitation = value / static_cast<float>(visits);
	float exploration = C * std::sqrt(std::log(static_cast<float>(dynamic_cast<MctsNode*>(this->parent)->getVisits())) /
			static_cast<float>(visits));
	return exploitation + exploration;
}

void MctsNode::addChild(Move move) {
	this->children.push_back(std::unique_ptr<MctsNode>(new MctsNode(this, move, this->getPos(), this->searchInfo)));
}