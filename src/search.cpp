
#include <iostream>
#include <limits>
#include <algorithm>
#include <atomic>
#include <chrono>

#include "search.hpp"
#include "uci.hpp"
#include "utils.hpp"
#include "sicario.hpp"

void Searcher::printMovesInformation() {
    for (auto pair : this->root->children) {
        printMove(std::get<1>(pair), true);
        NodeInfo& nodeInfo = this->getNodeInfo(std::get<0>(pair)->hash);
        std::cout << " " << nodeInfo.visits << " " << nodeInfo.value << " " <<
                std::get<0>(pair)->UCB1() << '\n';
    }

    std::tuple<Node*, Move> bestChild = this->root->bestChild();
    std::cout << "Best move: " << '\n';
    printMove(std::get<1>(bestChild), true);
    std::cout << '\n';
}

NodeInfo::NodeInfo(Searcher* searcher) {
    this->turn = searcher->position.getTurn();
}

Node::Node(Node* parent, Searcher* searcher) {
    this->parent = parent;
    this->hash = searcher->position.getHash();
    this->searcher = searcher;
    // this->turn = searcher->position.getTurn();

    if (searcher->nodes.find(this->hash) == searcher->nodes.end())
        searcher->nodes.insert(std::make_pair(this->hash, NodeInfo(searcher)));

    assert(parent == nullptr ? true : this->parent->hash != searcher->position.getHash());
}

Node::~Node() {
    for (auto pair : children)
        delete std::get<0>(pair);
}

void Node::addChild(Move move) {
    searcher->info.nodes++;
    searcher->info.nps_nodeCount++;
    this->searcher->position.processMakeMove(move);
    children.push_back(std::make_pair(new Node(this, searcher), move));
    this->searcher->position.processUndoMove();
}

float Node::UCB1() const {
    NodeInfo& nodeInfo = this->searcher->getNodeInfo(this->hash);
    NodeInfo& parentNodeInfo = this->searcher->getNodeInfo(this->parent->hash);
    if (nodeInfo.visits == 0) return std::numeric_limits<float>::max();
    return (static_cast<float>(nodeInfo.value) / static_cast<float>(nodeInfo.visits)) + C *
            std::sqrt(std::log(static_cast<float>(parentNodeInfo.visits)) / static_cast<float>(nodeInfo.visits));
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
    NodeInfo& nodeInfo = this->searcher->getNodeInfo(this->hash);
    MoveList moves = MoveList(searcher->position);
    if (nodeInfo.visits == 0 || searcher->position.isEOG(moves)) return this;

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
        NodeInfo& nodeInfo = this->searcher->getNodeInfo(curr->hash);
        nodeInfo.visits++;
        nodeInfo.value += nodeInfo.turn == this->searcher->rootPlayer ? -1 * val : val;
        curr = curr->parent;
    } while (curr != nullptr);
}

Node* Searcher::initialise(Position& position) {
    Node* root = new Node(nullptr, this);
    this->root = root;
    return root;
}

Searcher::Searcher(Position position, const std::atomic_bool& searchTree) :
        rootPlayer{position.getTurn()},
        searchTree{searchTree} {
    this->position = position;
    this->lastMessage = std::chrono::high_resolution_clock::now();
}

void Searcher::search() {
    Node* root = initialise(position);
    while (searchTree) {
        Node* leaf = root->select();
        leaf = leaf->expand();
        float val = leaf->simulate();
        leaf->rollback(val);
        printInfo();
    }
    printBestMove();
    // printMovesInformation(root);

    delete root;
}

NodeInfo& Searcher::getNodeInfo(Hash hash) {
    assert(this->nodes.find(hash) != this->nodes.end());
    return this->nodes.find(hash)->second;
}

void Sicario::mcts() {
    Searcher searcher(getPosition(), searchTree);
    searcher.search();
}

void Searcher::printInfo() { // TODO make this in line with the uci commands in the Sicario class
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

void Searcher::printBestMove() {
    std::tuple<Node*, Move> bestChild = this->root->bestChild();
    std::cout << "bestmove ";
    printMove(std::get<1>(bestChild), false, true);
    std::cout << '\n';
}
