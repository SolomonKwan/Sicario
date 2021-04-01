
#include <iostream>
#include <algorithm>

#include "mcts.hpp"

Node::Node(Pos& pos, bool root) : pos(pos) {
    this->isRoot = root;
}

float Node::UCB1() const {
    return 0.0;
}

Node& Node::select() {
    if (this->children.size() == 0) {
        return *this;
    }

    Node* node;
    auto comp = [](const Node& a, const Node& b) {
        return a.UCB1() < b.UCB1();
    };
    std::vector<Node>::iterator start = this->children.begin(), end = this->children.end();
    if (this->pos.getTurn() == WHITE) {
        node = &*std::max_element(start, end, comp);
    } else {
        node = &*std::min_element(start, end, comp);
    }
    return node->select();
}

void Node::expand() {
    
}

float Node::simulate() {
    return 0;
}

void Node::rollback() {

}

void Pos::mcst(SearchParams sp, std::atomic_bool& stop) {
    Node root = Node(*this, true);
    // std::cout << "Root hash: " << root.pos.getHash() << "\n";
    while (!stop) {
        Node leaf = root.select();
        // std::cout << "Chosen node hash: " << leaf.pos.getHash() << "\n";
        leaf.simulate();
        leaf.rollback();
        break;
    }
}
