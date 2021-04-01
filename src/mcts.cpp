
#include <iostream>
#include <algorithm>
#include <stack>

#include "mcts.hpp"

int Node::totalVisits = 0;

Node::Node(Pos& pos, bool root) {
    this->hash = pos.getHash();
    this->isRoot = root;
}

float Node::UCB1() const {
    return 0.0;
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
    if (pos.getTurn() == WHITE) {
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

Node& Node::expand() {
    
}

float Node::simulate() {
    return 0;
}

void Node::rollback() {

}

Node initialise(Pos& pos) {
    Node root = Node(pos, true);
    return root;
}

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop) {
    std::stack<Move> moveStack;
    Node root = initialise(pos);
    while (!stop) {
        Node leaf = root.select(pos, moveStack);
        leaf = leaf.expand();
        leaf.simulate();
        leaf.rollback();
    }
}
