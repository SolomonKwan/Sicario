
#include <iostream>
#include <algorithm>
#include <stack>
#include <random>

#include "mcts.hpp"

// This value is properly set in the initialise function.
Player Node::rootPlayer = WHITE;

/**
 * Node class constructor.
 * @param incoming_move: The move that got to this position.
 * @param root: Boolean indicating whether this is the root node.
 * @param hash: Hash of the current position.
 */
Node::Node(Move incoming_move, bool is_root, Hash hash) {
    this->is_root = is_root;
    this->hash = hash;
    this->incoming_move = incoming_move;
}

/**
 * Calculates the UCB1 value of the node.
 */
float Node::UCB1() const {
    if (this->visits == 0) return INFINITY;
    return (this->value / this->visits) + this->c * std::sqrt(std::log(this->parent->visits) / this->visits);
}

/**
 * Performs the selection phase of monte carlo tree search.
 * @param pos: Position from which to select a child (initial call should always be the root position).
 */
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

/**
 * Performs the expansion phase of monte carlo tree search.
 * @param pos: Position to expand from.
 * @param allocated_nodes: Vector to add allocated node pointers.
 */
Node* Node::expand(Pos& pos, std::vector<Node*>& allocated_nodes) {
    MoveList moves = MoveList(pos);
    if (this->visits == 0 || pos.isEOG(moves)) {
        return this;
    }

    // Expand the node
    for (Move move : moves) {
        pos.makeMove(move);
        Node* newNode = new Node(move);
        allocated_nodes.push_back(newNode);
        this->children.push_back(newNode);
        newNode->parent = this;
        pos.undoMove();
    }

    // Return a random child of the expanded node to be used in the next phase.
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> randomIndex(0, this->children.size() - 1);
    int index = randomIndex(rng);
    pos.makeMove(this->children[index]->incoming_move);
    return this->children[index];
}

/**
 * Performs the simulation step of monte carlo tree search.
 * @param pos: Position from which to perform a simulation.
 */
float Node::simulate(Pos& pos) {
    int moveCount = 0;
    MoveList moves = MoveList(pos);
    ExitCode code;

    // Perform the simulation.
    while (!(code = pos.isEOG(moves))) {
        pos.makeMove(moves.randomMove());
        moveCount++;
        moves = MoveList(pos);
    }

    // Undo the moves of the simulation.
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

/**
 * Performs rollback step of the monte carlo tree search.
 * @param val: The result to propagate back to the root.
 * @param pos: Position from which the simulation was made.
 */
void Node::rollback(float val, Pos& pos) {
    Node* curr = this;
    while (!curr->is_root) {
        curr->visits += 1;
        curr->value += (pos.getTurn() == Node::rootPlayer ? -1.0 * val : val);
        curr = curr->parent;
        pos.undoMove();
    }

    // Update the root node itself.
    curr->visits += 1;
    curr->value += (pos.getTurn() == Node::rootPlayer ? -1.0 * val : val);
}

/**
 * Creates the initial root node of the search and expands the root to its children.
 * @param pos: Position that is at the root node.
 * @param allocated_nodes: Vector to add allocated node pointers.
 */
Node* initialise(Pos& pos, std::vector<Node*>& allocated_nodes) {
    Node* root = new Node(0, true);
    allocated_nodes.push_back(root);
    MoveList moves = MoveList(pos);

    // Expand the root node.
    for (Move move : moves) {
        Node* newNode = new Node(move);
        allocated_nodes.push_back(newNode);
        root->children.push_back(newNode);
        newNode->parent = root;
    }

    // Set the root node player for rollback phase.
    Node::rootPlayer = pos.getTurn() == WHITE ? WHITE : BLACK;
    return root;
}

/**
 * Performs monte carlo tree search on the current position.
 * @param pos: Position on which to perform monte carlo tree search.
 * @param sp: Search parameters.
 * @param stop: Boolean indicating whether or not to continue the search.
 */
void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop) {
    std::vector<Node*> allocated_nodes;
    Node* root = initialise(pos, allocated_nodes);
    while (!stop) {
        Node* leaf = root->select(pos);
        leaf = leaf->expand(pos, allocated_nodes);
        float val = leaf->simulate(pos);
        leaf->rollback(val, pos);
    }

    std::cout << "Best move is ";
    auto comp = [](const Node* a, const Node* b) {
        return a->UCB1() < b->UCB1();
    };
    std::vector<Node*>::iterator start = root->children.begin(), end = root->children.end();
    Node* node = *std::max_element(start, end, comp);
    printMove(node->incoming_move, false);
    std::cout << "\n\n";

    for (Node* node : root->children) {
        printMove(node->incoming_move, false);
        std::cout << " " << node->UCB1() << " " << node->value << " " << node->visits << "\n";
    }

    // Free the allocated nodes.
    for (Node* node : allocated_nodes) {
        delete node;
    }

    // NEED TO FREE ALL THE MEMORY FOR THE NODES
}
