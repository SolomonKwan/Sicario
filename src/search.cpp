
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

void printMovesInformation(std::unique_ptr<Node>& root) {
    for (Node* node : root->children) {
        printMove(node->inEdge, true);
        std::cout << " " << node->visits << " " << node->value << " " << node->UCB1() << '\n';
    }

    Node* bestChild = *std::max_element(root->children.begin(), root->children.end(), Node::UCB1Comparator());
    std::cout << "Best move: " << '\n';
    printMove(bestChild->inEdge, true);
    std::cout << '\n';
}

Node::Node(Move move, Node* parent) {
    this->inEdge = move;
    this->parent = parent;
}

inline void Node::addChild(Move move, Searcher* searcher) {
    searcher->info.nodes++;
    searcher->info.nps_nodeCount++;
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
        addChild(move, searcher);
    }

    searcher->position.processMakeMove(children[0]->inEdge);
    return children[0]; // NOTE currently just getting the first child.
}

// TODO see if faster to make and undo move or if faster to create a copy of the object instead
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
    return std::make_unique<Node>(NULL_MOVE, nullptr);
}

Searcher::Searcher(Position position, const std::atomic_bool& searchTree) :
        rootPlayer{position.getTurn()},
        searchTree{searchTree} {
    this->position = position;
    this->lastMessage = std::chrono::high_resolution_clock::now();
}

void Searcher::search() {
    std::unique_ptr<Node> root = initialise(position);
    while (searchTree) {
        Node* leaf = root->select(this);
        leaf = leaf->expand(this);
        float val = leaf->simulate(this);
        leaf->rollback(this, val);
        printInfo(root);
    }
    printBestMove(root);
    // printMovesInformation(root);

    root->freeChildren();
}

void Sicario::mcts() {
    Searcher searcher(getPosition(), searchTree);
    searcher.search();
}

void Searcher::printInfo(std::unique_ptr<Node, std::default_delete<Node>>& root) { // TODO make this in line with the uci commands in the Sicario class
    std::chrono::time_point now = std::chrono::high_resolution_clock::now();
    uint64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMessage).count();
    if (duration >= 500) {
        std::cout << "nodes " << info.nodes;
        std::cout << " hashfull " << info.hashfull;
        std::cout << " nps " << static_cast<uint>(static_cast<float>(info.nps_nodeCount) / (static_cast<float>(duration) / 1000)) << '\n';
        this->lastMessage = now;
        info.nps_nodeCount = 0;
    }
}

void Searcher::printBestMove(std::unique_ptr<Node, std::default_delete<Node>>& root) {
    Node* bestChild = *std::max_element(root->children.begin(), root->children.end(), Node::UCB1Comparator());
    std::cout << "bestmove ";
    printMove(bestChild->inEdge, false, true);
    std::cout << '\n';
}
