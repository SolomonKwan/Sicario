
#include <iostream>
#include <algorithm>
#include <stack>
#include <random>

#include "mcts.hpp"

int Node::totalVisits = 0;

Node::Node(bool root) {
    this->isRoot = root;
}

float Node::UCB1() const {
    return (this->value / this->visits) + this->c * std::sqrt(std::log(Node::totalVisits) / this->visits);
}

Node& Node::select(Pos& pos, std::stack<Move>& moveStack) {
    if (this->children.size() == 0) {
        return *this;
    }

    auto comp = [](const std::pair<Node, Edge>& a, const std::pair<Node, Edge>& b) {
        return a.first.UCB1() < b.first.UCB1();
    };
    std::vector<std::pair<Node, Edge>>::iterator start = this->children.begin(), end = this->children.end();

    Node* node;
    if (pos.getTurn() == WHITE) { // TODO Return random if multiple are max, likewise for min.
        auto pair = std::max_element(start, end, comp);
        node = &(pair->first);
        pos.makeMove(pair->second);
        moveStack.push(pair->second);
    } else {
        auto pair = std::min_element(start, end, comp);
        node = &(pair->first);
        pos.makeMove(pair->second);
        moveStack.push(pair->second);
    }
    return node->select(pos, moveStack);
}

/**
 * TODO: Consider cases of multiple paths to same state.
 */
Node& Node::expand(Pos& pos, std::stack<Move>& moveStack, std::unordered_map<Hash, Node>& nodeMap) {
    // if (this->visits == 0) {
    //     return *this;
    // }

    // Expand the node
    MoveList moves = MoveList(pos);
    for (Move move : moves) {
        pos.makeMove(move);
        Node newNode = Node();
        this->children.push_back(std::make_pair(newNode, (Edge) move));
        newNode.parents.push_back(std::make_pair(*this, (Edge) move));
        pos.undoMove();
    }

    // Return the best node. For now, choose first one
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> random(0, this->children.size() - 1);
    int index = random(rng);
    
    std::cout << this << "\n";
    std::cout << this->children.size() << "\n";

    Node* addr = &this->children[index].first;
    std::cout << addr << "\n";
    return this->children[index].first;
}

float Node::simulate(Pos& pos) {
    int moveCount = 0;
    MoveList moves = MoveList(pos);
    ExitCode code;
    while (!(code = pos.isEOG(moves))) {
        pos.makeMove(moves.randomMove());
        moveCount++;
        moves = MoveList(pos);
    }

    while (moveCount != 0) {
        pos.undoMove();
        moveCount--;
    }

    if (code == BLACK_WINS) {
        return -1.0;
    } else if (code == WHITE_WINS) {
        return 1.0;
    }
    return 0.0;
}

void Node::rollback(float val, Pos& pos) {
    while (!this->isRoot) {
        this->visits += 1;
        this->value += val;
        *this = this->parents[0].first;
        pos.undoMove();
        
    }
    this->visits += 1;
    this->value += val;
}

Node initialise(Pos& pos, std::unordered_map<Hash, Node>& nodeMap) {
    Node root = Node(true);
    nodeMap.insert(std::make_pair(pos.getHash(), root));
    return root;
}

void Node::resetTotalCount() {
    Node::totalVisits = 0;
}

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop) {
    Node::resetTotalCount();
    std::stack<Move> moveStack;
    std::unordered_map<Hash, Node> nodeMap;
    Node root = initialise(pos, nodeMap);
    while (!stop) {
        std::cout << &root << "\n";
        Node& leaf = root.select(pos, moveStack);
        std::cout << &leaf << "\n";
        leaf = leaf.expand(pos, moveStack, nodeMap);
        std::cout << &leaf << "\n";
        std::cout << &root << "\n";
        std::cout << root.children.size() << "\n";
        // float val = leaf.simulate(pos);
        // leaf.rollback(val, pos);
        break;
    }
}
