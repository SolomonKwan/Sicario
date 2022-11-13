
#include <iostream>
#include <limits>
#include <algorithm>
#include <atomic>
#include <chrono>

#include "search.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"
#include "evaluate.hpp"

Node::Node(Node* parent, Searcher* searcher, Player turn, bool isRoot = false) {
	this->parent = parent;
	this->searcher = searcher;
	this->isRoot = isRoot;
	this->turn = turn;
}

Node::~Node() {
	for (auto pair : children)
		delete std::get<0>(pair);
}

void Node::addChild(Move move) {
	children.push_back(std::make_pair(new Node(this, searcher, !searcher->position.getTurn()), move));
}

float Node::UCB1() const {
	if (this->visits == 0) return std::numeric_limits<float>::max();
	return (static_cast<float>(this->value) / static_cast<float>(this->visits)) + std::sqrt(2) *
			std::sqrt(std::log(static_cast<float>(this->parent->visits)) / static_cast<float>(this->visits));
}

std::tuple<Node*, Move> Node::bestChild() {
	auto bestChild = *std::max_element(this->children.begin(), this->children.end(), Node::UCB1Comparator()); // NOTE this currently just chooses the last one it comes across if there are multiple of equal value
	return bestChild;
}

Node* Node::select() {
	if (children.size() == 0) return this;
	std::tuple<Node*, Move> bestChild = this->bestChild();
	searcher->position.processMakeMove(std::get<1>(bestChild));
	return std::get<0>(bestChild)->select();
}

Node* Node::expand() {
	MoveList moves = MoveList(searcher->position);
	if (this->visits == 0 || searcher->position.isEOG(moves)) return this;

	for (Move move : moves)
		addChild(move);

	this->searcher->position.processMakeMove(std::get<1>(children[0]));
	return std::get<0>(children[0]); // NOTE currently just getting the first child.
}

// TODO see if faster to make and undo move or if faster to create a copy of the object instead
float Node::simulate() {
	Position posCopy = this->searcher->position;
	MoveList moves = MoveList(posCopy);
	ExitCode code;
	while (!(code = posCopy.isEOG(moves))) {
		posCopy.processMakeMove(moves.randomMove());
		moves = MoveList(posCopy);
	}

	// Roll back original position object to root position
	while (searcher->position.getHistory().size() != 0)
		searcher->position.processUndoMove();

	if (code == WHITE_WINS) {
		return searcher->rootPlayer == WHITE ? 1 : -1;
	} else if (code == BLACK_WINS) {
		return searcher->rootPlayer == BLACK ? 1 : -1;
	}
	return 0;
}

void Node::rollback(float val) {
	Node* curr = this;
	do {
		curr->visits++;
		curr->value += curr->turn == this->searcher->rootPlayer ? -1 * val : val;
		curr = curr->parent;
	} while (curr != nullptr);
}

Searcher::Searcher(Position position) : rootPlayer{position.getTurn()} {
	this->position = position;
	this->lastMessage = std::chrono::high_resolution_clock::now();
	this->root = new Node(nullptr, this, this->position.getTurn(), true);
}

void Searcher::search(std::atomic_bool& searchTree) {
	while (searchTree) {
		Node* leaf = root->select();
		leaf = leaf->expand();
		float val = leaf->simulate();
		leaf->rollback(val);
	}
	printEndSearchInfo();
	delete root;
}

void Searcher::printEndSearchInfo() {
	for (auto pair : this->root->children) {
		printMove(std::get<1>(pair), true);
		if (type(std::get<1>(pair)) == NORMAL)
			std::cout << "\t";
		std::cout << "\tVisits: " << std::get<0>(pair)->visits << "\tValue: " << std::get<0>(pair)->value << '\n';
	}
}

void Sicario::mcts() {
	Searcher searcher(getPosition());
	searcher.search(this->searchTree);
}
