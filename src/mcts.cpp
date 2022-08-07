
#include <iostream>
#include <memory>
#include <limits>
#include <algorithm>
#include <atomic>

#include "mcts.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

Node::Node(Move move, Node* parent) {
    inEdge = move;
    parent = parent;
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

Node* Node::select(Position& position) {
    if (children.size() == 0) return this;
    Node* bestChild = *std::max_element(children.begin(), children.end(), Node::UCB1Comparator()); // NOTE this currently just chooses the last one it comes across if there are multiple of equal value
    position.processMakeMove(bestChild->getInEdge());
    return bestChild->select(position);
}

Node* Node::expand(Position& position) {
    MoveList moves = MoveList(position);
    if (visits == 0 || position.isEOG(moves)) return this;

    for (Move move : moves) {
        addChild(move);
    }

    position.processMakeMove(children[0]->inEdge);
    return children[0]; // NOTE currently just getting the first child.
}

float Node::simulate(Position& position, std::atomic_bool& searchTree, Player rootPlayer) {
    MoveList moves = MoveList(position);
    int moveCount = 0;
    while (!position.isEOG(moves)) {
        position.processMakeMove(moves.randomMove());
        moves = MoveList(position);
        moveCount++;
    }

    float result = 0.0;
    if (position.isEOG(moves) == WHITE_WINS) {
        result = rootPlayer == WHITE ? 1 : -1;
    } else if (position.isEOG(moves) == BLACK_WINS) {
        result = rootPlayer == BLACK ? 1 : -1;
    }

    while (moveCount > 0) {
        position.processUndoMove();
        moveCount--;
    }

    return result;
}

void Node::rollback(Position& position, float val, std::atomic_bool& searchTree, Player rootPlayer) {
    Node* curr = this;
    while (curr->parent != nullptr) {
        curr->value += position.getTurn() == rootPlayer ? val : -1 * val;
        curr->visits++;
        curr = curr->parent;
        position.processUndoMove();
    }
}

std::unique_ptr<Node> initialise(Position& position) {
    return std::make_unique<Node>(NULL_MOVE, nullptr);
}

Searcher::Searcher(Position position) : rootPlayer{position.getTurn()} {
    position = position;
}

void Searcher::search(std::atomic_bool& searchTree) {
    std::unique_ptr<Node> root = initialise(position);

    while (searchTree) {
        Node* leaf = root->select(position);
        leaf = leaf->expand(position);
        float val = leaf->simulate(position, searchTree, rootPlayer);
        leaf->rollback(position, val, searchTree, rootPlayer);
    }

    root->freeChildren();
}

void Sicario::mcts() {
    Searcher searcher(getPosition());
    searcher.search(searchTree);
}
