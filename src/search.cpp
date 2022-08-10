
#include <iostream>
#include <memory>
#include <limits>
#include <algorithm>
#include <atomic>
#include <chrono>

#include "search.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

Node::Node(Move move, Node* parent) {
    this->inEdge = move;
    this->parent = parent;
}

inline void Node::addChild(Move move) {
    children.push_back(new Node(move, this));
}

void Node::freeChildren() {
    if (children.size() == 0) return;
    for (Node* child : children) {
        child->freeChildren();
        delete child;
    }
}

Node* Node::select(Searcher* searcher) {
    if (children.size() == 0) return this;
    Node* bestChild = *std::max_element(children.begin(), children.end(), Node::UCB1Comparator()); // NOTE this currently just chooses the last one it comes across if there are multiple of equal value
    searcher->position.processMakeMove(bestChild->getInEdge());
    return bestChild->select(searcher);
}

Node* Node::expand(Searcher* searcher) {
    MoveList moves = MoveList(searcher->position);
    if (visits == 0 || searcher->position.isEOG(moves)) return this;

    for (Move move : moves) {
        addChild(move);
    }

    searcher->position.processMakeMove(children[0]->inEdge);
    return children[0]; // NOTE currently just getting the first child.
}

float Node::simulate(Searcher* searcher) {
    MoveList moves = MoveList(searcher->position);
    int moveCount = 0;
    ExitCode code;
    while (!(code = searcher->position.isEOG(moves))) {
        searcher->position.processMakeMove(moves.randomMove());
        moves = MoveList(searcher->position);
        moveCount++;
    }

    while (moveCount > 0) {
        searcher->position.processUndoMove();
        moveCount--;
    }

    if (code == WHITE_WINS) {
        return searcher->rootPlayer == WHITE ? 1 : -1;
    } else if (code == BLACK_WINS) {
        return searcher->rootPlayer == BLACK ? 1 : -1;
    }
    return 0;
}

void Node::rollback(Searcher* searcher, float val) {
    Node* curr = this;
    while (curr->parent != nullptr) {
        curr->visits++;
        curr->value += searcher->position.getTurn() == searcher->rootPlayer ? -1 * val : val;
        curr = curr->parent;
        searcher->position.processUndoMove();
    }

    curr->value += searcher->position.getTurn() == searcher->rootPlayer ? -1 * val : val;
    curr->visits++;
}

std::unique_ptr<Node> initialise(Position& position) {
    std::unique_ptr<Node> root = std::make_unique<Node>(NULL_MOVE, nullptr);
    for (Move move : MoveList(position)) {
        root->addChild(move);
    }
    return root;
}

Searcher::Searcher(Position position, const std::atomic_bool& searchTree) :
        rootPlayer{position.getTurn()},
        searchTree{searchTree} {
    this->position = position;
}

void Searcher::search() {
    GuiInfo info;

    std::unique_ptr<Node> root = initialise(position);
    while (searchTree) {
        Node* leaf = root->select(this);
        leaf = leaf->expand(this);
        float val = leaf->simulate(this);
        leaf->rollback(this, val);
    }

    root->freeChildren();
}

void Sicario::mcts() {
    Searcher searcher(getPosition(), searchTree);
    searcher.search();
}
