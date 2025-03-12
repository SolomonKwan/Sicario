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
	if (this->depth == depth)
		return;
	this->printInfo = true;
	this->depth = depth;
}

int SearchInfo::getSeldepth() const {
	return this->selDepth;
}

void SearchInfo::setSeldepth(int seldepth) {
	this->selDepth = seldepth;
}

void SearchInfo::setPrintInfo(bool printInfo) {
	this->printInfo = printInfo;
}

bool SearchInfo::getPrintInfo() const {
	return this->printInfo;
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

void SearchInfo::incrementNodes() {
	this->nodes++;
}

void Sicario::search() {
	Mcts mcts(this->getPosition(), this->searchTree, this->options);
	mcts.search();
	this->searchTree = false;
}

bool Mcts::rootIsEOG() {
	MoveList moves = MoveList(this->pos);
	if (this->pos.isCheckmate(moves)) {
		Uci::send("info depth 0 score mate 0");
		Uci::send("bestmove (none)");
		return true;
	} else if (this->pos.isDrawStalemate(moves) || this->pos.isDrawInsufficientMaterial()) {
		Uci::send("info depth 0 score cp 0");
		Uci::send("bestmove (none)");
		return true;
	} else if (this->pos.isDrawThreeFoldRep() || this->pos.isDrawFiftyMoveRule()) {
		Uci::send("info depth 0 score cp xxxx");
		Uci::send("bestmove (none)");
		return true;
	}
	return false;
}

// NOTE Due to way that the tree is constructed, it may result in stack overflow error due to node deletion/pruning.

void Mcts::search() {
	// Don't search if the game is already ended.
	if (this->rootIsEOG())
		return;

	SearchInfo searchInfo = SearchInfo();
	std::unique_ptr<Node> root(new Node(nullptr, NULL_MOVE, this->getPos(), searchInfo, this->options));
	root->rootInitialise();

	while (searchTree) {
		Node* leaf = root->select();
		leaf = leaf->expand();
		ExitCode code = leaf->simulate();
		leaf->rollback(code);

		if (searchInfo.getPrintInfo())
			Uci::sendInfo(searchInfo, root.get(), this->options);
	}

	Uci::sendInfo(searchInfo, root.get(), this->options); // Send final info command.
	Uci::sendBestMove(root.get(), options.debugMode);
}

Node* Node::select() {
	// Found leaf node. Set depth and return;
	if (this->children.size() == 0) {
		this->searchInfo.setDepth(this->depth);
		return this;
	}

	Node* bestChild = this->bestChild();

	// Only set currMove if we are at the root.
	if (this->parent == nullptr)
		searchInfo.setCurrMove(bestChild->getInEdge());

	this->getPos().makeMove(bestChild->getInEdge());
	return bestChild->select();
}

Node* Node::expand() {
	// Return node if it has never been simulated from or if it's end of game.
	MoveList moves = MoveList(this->getPos());
	if (visits == 0 || this->getPos().isEOG(moves))
		return this;

	// Expand the node.
	for (Move move : moves)
		this->addChild(move);

	// Choose a child node (NOTE currenty just choosing the first one.).
	this->getPos().makeMove(this->children[0]->getInEdge());
	return this->children[0].get();
}

ExitCode Node::simulate() {
	ExitCode code;
	int moveCount = 0;
	MoveList moves = MoveList(this->pos);
	while (!(code = this->pos.isEOG(moves))) {
		this->pos.makeMove(moves.randomMove());
		moves = MoveList(this->pos);
		moveCount++;
	}

	// Increment node count if this is the first simulation for the node.
	if (this->getVisits() == 0)
		this->searchInfo.incrementNodes();

	// TODO could potentially create some kind of "copy" method for the Position object which we can copy only the information we need and then discard removing the need for this undo loop. Would need to test the performance.
	for (; moveCount > 0; moveCount--)
		this->pos.undoMove();

	return code;
}

void Node::rollback(ExitCode code) {
	bool whiteRootWins = (code == WHITE_WINS && this->pos.getOriginalTurn() == WHITE);
	bool blackRootWins = (code == BLACK_WINS && this->pos.getOriginalTurn() == BLACK);

	Node* curr = this;
	while (curr != nullptr) {
		curr->visits++;

		// NOTE: Side to move in a checkmate position is the side that lost.
		if ((whiteRootWins && this->pos.getTurn() == BLACK) || (blackRootWins && this->pos.getTurn() == WHITE)) {
			curr->value += 1;
		} else if (code == STALEMATE || code == THREE_FOLD_REPETITION || code == FIFTY_MOVES_RULE ||
				code == INSUFFICIENT_MATERIAL) {
			curr->value += 0.5;
		}

		curr = curr->parent;
		if (curr != nullptr)
			this->pos.undoMove();
	}
}

Node* Node::bestChild() {
	if (this->children.size() == 0) return nullptr;
	std::vector<Node*> ptrs;
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

Node* Node::bestChildPv(int pvLine) {
	std::map<float, std::vector<Node*>> map;
	for (auto& child : this->children)
		map[child->Ucb1()].push_back(child.get());

	Node* node = nullptr;
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

void Node::rootInitialise() {
	assert(this->parent == nullptr);
	MoveList moves = MoveList(this->getPos());
	for (Move move : moves)
		this->addChild(move);
}

const std::vector<Node*> Node::getChildren() const {
	std::vector<Node*> children;
	for (auto& child : this->children)
		children.push_back(child.get());
	return children;
}

float Node::Ucb1() const {
	if (this->visits == 0) return std::numeric_limits<float>::max();
	return (value / static_cast<float>(visits)) +
			std::sqrt(static_cast<double>(this->options.ExplorationOption.getValue() / 1000)) *
			std::sqrt(std::log(static_cast<float>(this->parent->getVisits())) /
			static_cast<float>(visits));
}

void Node::addChild(Move move) {
	this->children.push_back(std::unique_ptr<Node>(new Node(this, move, this->getPos(), this->searchInfo, this->options)));
}