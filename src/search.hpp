
#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <atomic>
#include <chrono>

#include "game.hpp"
#include "constants.hpp"

class Searcher;

class Node {
	public:
		Node(Node* parent, Searcher* searcher, Player turn, bool isRoot);

		~Node();

		void addChild(Move move);

		float UCB1() const;

		std::tuple<Node*, Move> bestChild();

		Node* select();

		Node* expand();

		float simulate();

		void rollback(float val);

		struct UCB1Comparator {
			bool operator ()(const std::tuple<Node*, Move> a, const std::tuple<Node*, Move> b) const {
				return std::get<0>(a)->UCB1() < std::get<0>(b)->UCB1();
			}
		};

		bool isRoot;
		Searcher* searcher;
		Node* parent = nullptr;
		std::vector<std::tuple<Node*, Move>> children;
		int value = 0;
		uint visits = 0;
		Player turn;
};

class Searcher {
	public:
		Searcher(Position position);
		void search(std::atomic_bool&);
		void printEndSearchInfo();

		Node* root;
		Position position;
		const Player rootPlayer;
		std::chrono::_V2::system_clock::time_point lastMessage;
};

#endif
