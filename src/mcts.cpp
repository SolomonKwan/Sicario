
#include <iostream>
#include <algorithm>
#include <stack>

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

Node& Node::expand(Pos& pos, std::stack<Move>& moveStack, std::unordered_map<Hash, Node>& nodeMap) {
    if (this->visits == 0) {
        return *this;
    }

    // Expand the node
    MoveList moves = MoveList(pos);
    for (Move move : moves) {
        pos.makeMove(move);

        auto pair = nodeMap.find(pos.getHash());
        if (pair != nodeMap.end()) {
            this->children.push_back(std::make_pair(pair->second, (Edge) move));
            pair->second.parents.push_back(std::make_pair(*this, (Edge) move));
        } else {
            Node newNode = Node();
            this->children.push_back(std::make_pair(newNode, (Edge) move));
            newNode.parents.push_back(std::make_pair(*this, (Edge) move));
        }

        pos.undoMove();
    }

    // Return the best node. For now, choose first one
    return this->children[0].first;
}

float Node::simulate() {
    return 0;
}

void Node::rollback() {

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
        Node leaf = root.select(pos, moveStack);
        leaf = leaf.expand(pos, moveStack, nodeMap);
        leaf.simulate();
    //     leaf.rollback();
    }
}
