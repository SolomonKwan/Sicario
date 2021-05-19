
#include <iostream>
#include <algorithm>
#include <stack>
#include <random>
#include <unordered_set>

#include "mcts.hpp"

// This value is properly set in the initialise function.
Player Node::rootPlayer = WHITE;

/**
 * Node class constructor.
 * @param incoming_move: The move that got to this position.
 * @param root: Boolean indicating whether this is the root node.
 * @param hash: Hash of the current position.
 */
Node::Node(Move incoming_move, bool is_root, Hash hash, bool turn, int depth) {
    this->is_root = is_root;
    this->hash = hash;
    this->incoming_move = incoming_move;
    this->turn = turn;
    this->depth = depth;
}

/**
 * Calculates the UCB1 value of the node.
 */
float Node::UCB1(SearchParams& sp) const {
    if (this->visits == 0) return INFINITY;
    return (this->value / this->visits) + sp.c * std::sqrt(std::log(this->parent->visits) / this->visits);
}

/**
 * Performs the selection phase of monte carlo tree search.
 * @param pos: Position from which to select a child (initial call should always be the root position).
 */
Node* Node::select(Pos& pos, SearchParams& sp) {
    if (this->children.size() == 0) {
        return this;
    }
    Node* bestChild = this->bestChild(sp);
    pos.makeMove(bestChild->incoming_move);
    return bestChild->select(pos, sp);
}

/**
 * Performs the expansion phase of monte carlo tree search.
 * @param pos: Position to expand from.
 * @param nodes: Set of all nodes with the same hashes.
 */
Node* Node::expand(Pos& pos, std::unordered_map<Hash, std::unordered_set<Node*>>& nodes, SearchParams& sp) {
    MoveList moves = MoveList(pos);
    if (this->visits == 0 || pos.isEOG(moves)) {
        return this;
    }

    // Expand the node
    for (Move move : moves) {
        pos.makeMove(move);
        Node* newNode = new Node(move, false, pos.getHash(), pos.getTurn(), this->depth + 1);
        nodes[pos.getHash()].insert(newNode);
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
 * @param nodes: Set of all nodes with the same hashes.
 */
void Node::rollback(float val, Pos& pos, std::unordered_map<Hash, std::unordered_set<Node*>>& nodes) {
    Hash hash = pos.getHash();

    // Save current hash then undo move back to root.
    Node* curr = this;
    while (!curr->is_root) {
        curr = curr->parent;
        pos.undoMove();
    }

    // Rollback the value through every occurence of the hash position.
    for (Node* node : nodes[hash]) {
        while (!node->is_root) {
            node->visits += 1;
            node->value += (node->turn == Node::rootPlayer ? -1.0 * val : val);
            node = node->parent;
        }

        // Update the root node itself.
        node->visits += 1;
        node->value += (node->turn == Node::rootPlayer ? -1.0 * val : val);
    }
}

/**
 * Creates the initial root node of the search and expands the root to its children.
 * @param pos: Position that is at the root node.
 * @param nodes: Set of all nodes with the same hashes.
 */
Node* initialise(Pos& pos, std::unordered_map<Hash, std::unordered_set<Node*>>& nodes, SearchParams& sp) {
    Node* root = new Node(0, true, pos.getHash(), pos.getTurn(), 0);
    nodes[pos.getHash()].insert(root);
    MoveList moves = MoveList(pos);

    // Expand the root node.
    for (Move move : moves) {
        pos.makeMove(move);
        Node* newNode = new Node(move, false, pos.getHash(), pos.getTurn(), 1);
        nodes[pos.getHash()].insert(newNode);
        root->children.push_back(newNode);
        newNode->parent = root;
        pos.undoMove();
    }

    // Set the root node player for rollback phase.
    Node::rootPlayer = pos.getTurn() == WHITE ? WHITE : BLACK;
    return root;
}

Node* Node::bestChild(SearchParams& sp) {
    float max_val = -INFINITY;
    std::vector<Node*> maximal_nodes;
    for (Node* node : this->children) {
        float val = node->UCB1(sp);
        if (val > max_val) {
            maximal_nodes.clear();
            max_val = val;
            maximal_nodes.push_back(node);
        } else if (val == max_val) {
            maximal_nodes.push_back(node);
        }
    }
    if (maximal_nodes.size() == 0) return nullptr;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> randomIndex(0, maximal_nodes.size() - 1);
    return maximal_nodes[randomIndex(rng)];
}

/**
 * Prints the info of the search information.
 */
void printInfo(Node* root, Info info) {
    // Move bestMove = root->bestChild()->incoming_move;
    // if (bestMove != info.bestMove) {
    //     std::cout <<
    //             "info depth " << info.depth <<
    //             " seldepth " << info.seldepth <<
    //             " multipv " << info.multipv <<
    //             " score cp " << info.cp <<
    //             " nodes " << info.nodes <<
    //             " nps " << info.nps <<
    //             " tbhits " << info.tbhits <<
    //             " time " << info.time <<
    //             " pv ";
    //     printMove(bestMove, false);
    //     std::cout << "\n";
    //     info.bestMove = bestMove;
    // }
}

/**
 * Print the best move after end of search.
 */
void printBestMove(Node* root, SearchParams& sp) {
    // Print the bestmove
    std::cout << "bestmove ";
    Node* bestNode = root->bestChild(sp);
    if (bestNode == nullptr) {
        std::cout << "(None)" << "\n";
    } else {
        printMove(bestNode->incoming_move, false);
    }
    std::cout << "\n";

    // Print debug stuff
    for (Node* node : root->children) {
        printMove(node->incoming_move, false);
        std::cout << " " << node->UCB1(sp) << " " << node->value << " " << node->visits << "\n";
    }

    // Print pondermove

    std::cout << "\n";
}

/**
 * Performs monte carlo tree search on the current position.
 * @param pos: Position on which to perform monte carlo tree search.
 * @param sp: Search parameters.
 * @param stop: Boolean indicating whether or not to continue the search.
 */
void mcts(Pos& pos, SearchParams sp, std::atomic_bool& stop) {
    Info info;
    std::unordered_map<Hash, std::unordered_set<Node*>> nodes;
    Node* root = initialise(pos, nodes, sp);
    while (!stop) {
        Node* leaf = root->select(pos, sp);
        leaf = leaf->expand(pos, nodes, sp);
        float val = leaf->simulate(pos);
        leaf->rollback(val, pos, nodes);
        printInfo(root, info);
    }
    printBestMove(root, sp);
    stop = true;

    // Free the allocated nodes.
    for (auto set : nodes) {
        for (Node* node : set.second) {
            delete node;
        }
    }
}
