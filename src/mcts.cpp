
#include <iostream>
#include <algorithm>
#include <stack>
#include <random>

#include "mcts.hpp"

Player Node::rootPlayer = WHITE;

Node::Node(Move incoming_move, bool root, Hash hash) {
    this->isRoot = root;
    this->hash = hash;
    this->incoming_move = incoming_move;
}

float Node::UCB1() const {
    if (this->visits == 0) return INFINITY;
    return (this->value / this->visits) + this->c * std::sqrt(std::log(this->parent->visits) / this->visits);
}

Node* Node::select(Pos& pos) {
    if (this->children.size() == 0) {
        return this;
    }

    float max_val = -INFINITY;
    std::vector<Node*> maximal_nodes;
    for (Node* node : this->children) {
        float val = node->UCB1();
        if (val > max_val) {
            maximal_nodes.clear();
            max_val = val;
            maximal_nodes.push_back(node);
        } else if (val == max_val) {
            maximal_nodes.push_back(node);
        }
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> randomIndex(0, maximal_nodes.size() - 1);
    int index = randomIndex(rng);
    pos.makeMove(maximal_nodes[index]->incoming_move);
    return maximal_nodes[index]->select(pos);
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

    if ((Node::rootPlayer == WHITE && code == WHITE_WINS) || (Node::rootPlayer == BLACK && code == BLACK_WINS)) {
        return 1.0;
    } else if ((Node::rootPlayer == BLACK && code == WHITE_WINS) || (Node::rootPlayer == WHITE && code == BLACK_WINS)) {
        return -1.0;
    }
    return 0.0;
}

void Node::rollback(float val, Pos& pos) {
    Node* curr = this;
    while (!curr->isRoot) {
        curr->visits += 1;
        curr->value += (pos.getTurn() == Node::rootPlayer ? -1.0 * val : val);
        curr = curr->parent;
        pos.undoMove();
    }
    curr->visits += 1;
    curr->value += (pos.getTurn() == Node::rootPlayer ? -1.0 * val : val);
}

Node* initialise(Pos& pos) {
    Node* root = new Node(0, true);
    MoveList moves = MoveList(pos);
    for (Move move : moves) {
        Node* newNode = new Node(move);
        root->children.push_back(newNode);
        newNode->parent = root;
    }
    Node::rootPlayer = pos.getTurn() == WHITE ? WHITE : BLACK;
    return root;
}

void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop) {
    pos.parseFen("k7/pp6/8/8/8/8/8/4K2R w - - 0 1");
    Node* root = initialise(pos);
    while (!stop) {
        Node* leaf = root->select(pos);
        leaf = leaf->expand(pos);
        float val = leaf->simulate(pos);
        leaf->rollback(val, pos);
    }

    std::cout << "Best move is ";
    auto comp = [](const Node* a, const Node* b) {
        return a->UCB1() < b->UCB1();
    };
    std::vector<Node*>::iterator start = root->children.begin(), end = root->children.end();

    Node* node;
    if (pos.getTurn() == WHITE) {
        node = *std::max_element(start, end, comp);
        printMove(node->incoming_move, false);
    } else {
        node = *std::min_element(start, end, comp);
        printMove(node->incoming_move, false);
    }
    std::cout << "\n\n";

    for (Node* node : root->children) {
        printMove(node->incoming_move, false);
        std::cout << " " << node->UCB1() << " " << node->value << " " << node->visits << "\n";
    }

    // NEED TO FREE ALL THE MEMORY FOR THE NODES
}
