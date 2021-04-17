
#include <iostream>
#include <algorithm>
#include <stack>
#include <random>

#include "mcts.hpp"

float Node::totalVisits = 0;

Node::Node(Move incoming_move, bool root, Hash hash) {
    this->isRoot = root;
    this->hash = hash;
    this->incoming_move = incoming_move;
}

float Node::UCB1() const {
    return (this->value / this->visits) + this->c * std::sqrt(std::log(Node::totalVisits) / this->visits);
}

Node* Node::select(Pos& pos) {
    if (this->children.size() == 0) {
        return this;
    }

    auto comp = [](const Node* a, const Node* b) {
        return a->UCB1() < b->UCB1();
    };
    std::vector<Node*>::iterator start = this->children.begin(), end = this->children.end();

    Node* node;
    if (pos.getTurn() == WHITE) {
        node = *std::max_element(start, end, comp);
        pos.makeMove(node->incoming_move);
    } else {
        node = *std::min_element(start, end, comp);
        pos.makeMove(node->incoming_move);
    }
    return node->select(pos);
}

Node* Node::expand(Pos& pos) {
    MoveList moves = MoveList(pos);
    if (this->visits == 0 || pos.isEOG(moves)) {
        return this;
    }

    // Expand the node
    for (Move move : moves) {
        pos.makeMove(move);
        Node* newNode = new Node(move);
        this->children.push_back(newNode);
        newNode->parent = this;
        pos.undoMove();
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> randomIndex(0, this->children.size() - 1);
    int index = randomIndex(rng);
    pos.makeMove(this->children[index]->incoming_move);
    return this->children[index];
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
    Node* curr = this;
    while (!curr->isRoot) {
        curr->visits += 1;
        curr->value += val;
        curr = curr->parent;
        pos.undoMove();
    }
    curr->visits += 1;
    curr->value += val;
    Node::totalVisits += 1;
}

Node initialise(Pos& pos) {
    Node root = Node(0, true);
    return root;
}

void Node::resetTotalCount() {
    Node::totalVisits = 0;
}

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop) {
    Node::resetTotalCount();
    pos.parseFen("4k3/3ppp2/8/8/8/8/8/4K2R w - - 0 1");
    Node root = initialise(pos);
    while (!stop) {
        Node* leaf = root.select(pos);
        leaf = leaf->expand(pos);
        float val = leaf->simulate(pos);
        leaf->rollback(val, pos);
    }

    std::cout << "Best move is ";
    auto comp = [](const Node* a, const Node* b) {
        return a->UCB1() < b->UCB1();
    };
    std::vector<Node*>::iterator start = root.children.begin(), end = root.children.end();

    Node* node;
    if (pos.getTurn() == WHITE) {
        node = *std::max_element(start, end, comp);
        printMove(node->incoming_move, false);
    } else {
        node = *std::min_element(start, end, comp);
        printMove(node->incoming_move, false);
    }
    std::cout << "\n\n";

    for (Node* node : root.children) {
        printMove(node->incoming_move, false);
        std::cout << " " << node->UCB1() << " " << node->value << " " << node->visits << "\n";
    }

    std::cout << "\n" << root.children.size() << "\n";

    // float i = 0, j = 0;
    // float nan = i / j;
    // std::cout << nan << "\n";

    // i = -1;
    // j = 0;
    // float ninf = i / j;
    // std::cout << ninf << "\n";

    // i = 1;
    // j = 0;
    // float inf = i / j;
    // std::cout << inf << "\n";

    // i = 0;
    // j = 1;
    // float z = i / j;
    // std::cout << z << "\n";

    // i = 0;
    // j = -1;
    // float nz = i / j;
    // std::cout << nz << "\n\n";

    // std::cout << (nan + 0.0) << "\n";
    // std::cout << (nan + 2.0) << "\n";

    // // std::cout << (result1 < result2) << "\n";
    // // std::cout << (result2 < result3) << "\n";
    // // std::cout << (result1 < result3) << "\n";

    // // std::cout << (result1 > result2) << "\n";
    // // std::cout << (result2 > result3) << "\n";
    // // std::cout << (result1 > result3) << "\n";

    // NEED TO FREE ALL THE MEMORY FOR THE NODES
}
