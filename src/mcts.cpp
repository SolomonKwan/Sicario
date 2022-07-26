
#include <iostream>
#include <memory>
#include <limits>
#include <algorithm>
#include <atomic>

#include "mcts.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

// int newcount = 0;
// int deletecount = 0;

Node::Node(Move move, Node* parent) {
    inEdge = move;
    parent = parent;
}

inline void Node::addChild(Move move) {
    // newcount++;
    children.push_back(new Node(move, this));
}

void Node::freeChildren() {
    if (children.size() == 0) return;

    for (Node* child : children) {
        child->freeChildren();
        // deletecount++;
        delete child;
    }
}

Node* Node::select(Position& position) {
    if (children.size() == 0) return this;
    // NOTE this currently just chooses the last one it comes across if there are multiple of equal value
    Node* bestChild = *std::max_element(children.begin(), children.end(), Node::UCB1Comparator());
    position.processMakeMove(bestChild->getInEdge());
    return bestChild->select(position);
}

Node* Node::expand(Position& position) {
    MoveList moves = MoveList(position);
    if (visits == 0 || position.isEOG(moves)) return this;

    for (Move move : moves) {
        addChild(move);
    }

    return children[0]; // NOTE currently just getting the first child.
}

float Node::simulate(Position& position, std::atomic_bool& searchTree) {
    return 0.0;
}

void Node::rollback(Position& position, float val, std::atomic_bool& searchTree) {

}

std::unique_ptr<Node> initialise(Position& position) {
    std::unique_ptr<Node> root = std::make_unique<Node>(NULL_MOVE, nullptr);
    for (Move move : MoveList(position)) {
        root->addChild(move);
    }
    return root;
}

void Sicario::mcts() {
    Position positionCopy = getPosition();
    std::unique_ptr<Node> root = initialise(positionCopy);

    while (searchTree) {
        Node* leaf = root->select(positionCopy);
        leaf = leaf->expand(positionCopy);
        float val = leaf->simulate(positionCopy, searchTree);
        leaf->rollback(position, val, searchTree);
    }

    root->freeChildren();
    // std::cout << newcount << " " << deletecount << '\n';
}
