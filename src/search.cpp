
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
    ExitCode code;
    while (!(code = position.isEOG(moves))) {
        position.processMakeMove(moves.randomMove());
        moves = MoveList(position);
        moveCount++;
    }

    while (moveCount > 0) {
        position.processUndoMove();
        moveCount--;
    }

    if (code == WHITE_WINS) {
        return rootPlayer == WHITE ? 1 : -1;
    } else if (code == BLACK_WINS) {
        return rootPlayer == BLACK ? 1 : -1;
    }
    return 0;
}

void Node::rollback(Position& position, float val, std::atomic_bool& searchTree, Player rootPlayer) {
    Node* curr = this;
    while (curr->parent != nullptr) {
        curr->visits++;
        curr->value += position.getTurn() == rootPlayer ? -1 * val : val;
        curr = curr->parent;
        position.processUndoMove();
    }

    curr->value += position.getTurn() == rootPlayer ? -1 * val : val;
    curr->visits++;
}

std::unique_ptr<Node> initialise(Position& position) {
    std::unique_ptr<Node> root = std::make_unique<Node>(NULL_MOVE, nullptr);
    for (Move move : MoveList(position)) {
        root->addChild(move);
    }
    return root;
}

Searcher::Searcher(Position position) : rootPlayer{position.getTurn()} {
    this->position = position;
}

void Searcher::search(std::atomic_bool& searchTree) {
    std::unique_ptr<Node> root = initialise(position);

    while (searchTree) {
        Node* leaf = root->select(position);
        leaf = leaf->expand(position);
        float val = leaf->simulate(position, searchTree, rootPlayer);
        leaf->rollback(position, val, searchTree, rootPlayer);
    }

    for (Node* node : root->children) {
        printMove(node->inEdge, true);
        std::cout << '\t' << node->visits << '\t' << node->value << '\t' << node->UCB1() << '\n';
    }
    std::cout << "Best move: ";
    Node* bestChild = *std::max_element(root->children.begin(), root->children.end(), Node::UCB1Comparator());
    printMove(bestChild->inEdge, true);
    std::cout << '\n';

    root->freeChildren();
}

void Sicario::mcts() {
    Searcher searcher(getPosition());
    searcher.search(searchTree);
}
