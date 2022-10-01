
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <atomic>
#include <chrono>

#include "game.hpp"
#include "constants.hpp"

const float C = std::sqrt(2); // TODO make this a uci paramter for user to set.

// GUI info command data
struct GuiInfo {
	uint64_t nodes = 0;
	uint hashfull = 0;
	uint64_t nps_nodeCount = 0;
};

class Searcher;

struct NodeInfo {
	NodeInfo(Searcher* searcher);
	int value = 0;
	uint visits = 0;
	Player turn;
};

class Node {
	public:
		Node(Node* parent, Searcher* searcher, bool isRoot);

		~Node();

		void addChild(Move move);

		float UCB1() const;

		std::tuple<Node*, Move> bestChild();

		const std::vector<std::tuple<Node*, Move>>& getChildren() const;

		const Hash getHash() const;

		const Hash getSearcherHash() const;

		const uint getParentVisits() const;

		Node* select();

		Node* expand();

		float simulate();

		void rollback(float val);

		struct UCB1Comparator {
			bool operator ()(const std::tuple<Node*, Move> a, const std::tuple<Node*, Move> b) const {
				return std::get<0>(a)->UCB1() < std::get<0>(b)->UCB1();
			}
		};

	private:
		Hash hash;
		bool isRoot;
		Searcher* searcher;
		Node* parent = nullptr;
		std::vector<std::tuple<Node*, Move>> children;
};

class Searcher {
	public:
		Searcher(Position position, const std::atomic_bool& searchTree);
		void search();

		Node* root;
		GuiInfo guiInfo;
		Position position;
		const Player rootPlayer;
		const std::atomic_bool& searchTree;
		std::unordered_map<Hash, NodeInfo> nodeInfos;
		std::chrono::_V2::system_clock::time_point lastMessage;

		void printInfo();
		void printBestMove();
		Node* initialise(Position& position);
		NodeInfo& getNodeInfo(Hash hash);

		// NOTE Used for debugging
		void printMovesInformation();
};

#endif
