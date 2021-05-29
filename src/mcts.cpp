
#include <iostream>
#include <algorithm>
#include <stack>
#include <random>
#include <unordered_set>
#include <chrono>

#include "mcts.hpp"
#include "uci.hpp"

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
float Node::UCB1(Searcher& searcher) const {
    if (this->visits == 0) return INFINITY;
    return (this->value / this->visits) + searcher.c * std::sqrt(std::log(this->parent->visits) / this->visits);
}

/**
 * Performs the selection phase of monte carlo tree search.
 * @param pos: Position from which to select a child (initial call should always be the root position).
 */
Node* Node::select(Searcher& searcher) {
    if (this->children.size() == 0) {
        return this;
    }
    Node* bestChild = this->bestChild(searcher);
    searcher.pos.makeMove(bestChild->incoming_move);
    return bestChild->select(searcher);
}

/**
 * Performs the expansion phase of monte carlo tree search.
 * @param pos: Position to expand from.
 * @param nodes: Set of all nodes with the same hashes.
 */
Node* Node::expand(Searcher& searcher) {
    MoveList moves = MoveList(searcher.pos);
    if (this->visits == 0 || searcher.pos.isEOG(moves)) {
        return this;
    }

    // Expand the node
    for (Move move : moves) {
        searcher.pos.makeMove(move);
        Node* newNode = new Node(move, false, searcher.pos.getHash(), searcher.pos.getTurn(), this->depth + 1);
        searcher.nodes++;
        if (this->depth + 1 > searcher.depth) searcher.depth = this->depth + 1;
        searcher.hashPositions[searcher.pos.getHash()].insert(newNode);
        this->children.push_back(newNode);
        newNode->parent = this;
        searcher.pos.undoMove();
    }

    // Return a random child of the expanded node to be used in the next phase.
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> randomIndex(0, this->children.size() - 1);
    int index = randomIndex(rng);
    searcher.pos.makeMove(this->children[index]->incoming_move);
    return this->children[index];
}

/**
 * Performs the simulation step of monte carlo tree search.
 * @param pos: Position from which to perform a simulation.
 */
float Node::simulate(Searcher& searcher) {
    int moveCount = 0;
    MoveList moves = MoveList(searcher.pos);
    ExitCode code;

    // Perform the simulation.
    while (!(code = searcher.pos.isEOG(moves))) {
        searcher.pos.makeMove(searcher.pos.pseudoRandomMove(moves));
        moveCount++;
        moves = MoveList(searcher.pos);
    }

    // Undo the moves of the simulation.
    while (moveCount != 0) {
        searcher.pos.undoMove();
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
void Node::rollback(float val, Searcher& searcher) {
    Hash hash = searcher.pos.getHash();

    // Save current hash then undo move back to root.
    Node* curr = this;
    while (!curr->is_root) {
        curr = curr->parent;
        searcher.pos.undoMove();
    }

    // Rollback the value through every occurence of the hash position.
    for (Node* node : searcher.hashPositions[hash]) {
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
Node* initialise(Searcher& searcher) {
    Node* root = new Node(0, true, searcher.pos.getHash(), searcher.pos.getTurn(), 0);
    searcher.nodes++;
    searcher.hashPositions[searcher.pos.getHash()].insert(root);
    MoveList moves = MoveList(searcher.pos);

    // Expand the root node.
    for (Move move : moves) {
        searcher.pos.makeMove(move);
        Node* newNode = new Node(move, false, searcher.pos.getHash(), searcher.pos.getTurn(), 1);
        searcher.nodes++;
        if (1 > searcher.depth) searcher.depth = 1;
        searcher.hashPositions[searcher.pos.getHash()].insert(newNode);
        root->children.push_back(newNode);
        newNode->parent = root;
        searcher.pos.undoMove();
    }

    // Set the root node player for rollback phase.
    Node::rootPlayer = searcher.pos.getTurn() == WHITE ? WHITE : BLACK;
    return root;
}

Node* Node::bestChild(Searcher& searcher) {
    float max_val = -INFINITY;
    std::vector<Node*> maximal_nodes;
    for (Node* node : this->children) {
        float val = node->UCB1(searcher);
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
void printInfo(Node* root, Searcher& searcher) {
    auto now = std::chrono::high_resolution_clock::now();
    int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - searcher.start).count();
    int nps = (int) (searcher.nodes / (duration / (double) 1000));

    Move bestMove = root->bestChild(searcher)->incoming_move;
    if (bestMove != searcher.bestMove) {
        std::cout <<
                "info depth " << searcher.depth <<
                " seldepth " << searcher.seldepth <<
                " multipv " << searcher.multipv <<
                " score cp " << searcher.cp <<
                " nodes " << searcher.nodes <<
                " nps " << nps <<
                " tbhits " << searcher.tbhits <<
                " time " << duration <<
                " pv ";
        printMove(bestMove, false);
        std::cout << "\n";
        searcher.bestMove = bestMove;
    }
}

/**
 * Print the best move after end of search.
 */
void printBestMove(Node* root, Searcher searcher) {
    // Print the bestmove
    std::cout << "bestmove ";
    Node* bestNode = root->bestChild(searcher);
    if (bestNode == nullptr) {
        std::cout << "(None)" << "\n";
    } else {
        printMove(bestNode->incoming_move, false);
    }
    std::cout << "\n";

    // Print debug stuff
    for (Node* node : root->children) {
        printMove(node->incoming_move, false);
        std::cout << " " << node->UCB1(searcher) << " " << node->value << " " << node->visits << "\n";
    }

    // Print pondermove

    std::cout << "\n" << std::flush;
}

/**
 * Performs monte carlo tree search on the current position.
 * @param pos: Position on which to perform monte carlo tree search.
 * @param sp: Search parameters.
 * @param stop: Boolean indicating whether or not to continue the search.
 */
void Searcher::mcts(std::atomic_bool& stop, GoParams go_params) {
    Node* root = initialise(*this);
    while (!stop) {
        Node* leaf = root->select(*this);
        leaf = leaf->expand(*this);
        float val = leaf->simulate(*this);
        leaf->rollback(val, *this);
        // printInfo(root, *this);
    }
    printBestMove(root, *this);
    stop = true;

    // Free the allocated nodes.
    for (auto set : this->hashPositions) {
        for (Node* node : set.second) {
            delete node;
        }
    }
}
