#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <vector>
#include <memory>
#include <iostream>

#include "sicario.hpp"

class BaseSearcher {
	public:
		BaseSearcher(const Position& pos) : pos(pos), rootPlayer(pos.getTurn()) {}
		virtual void search(const std::atomic_bool& searchTree) = 0;
		Position& getPos();

	protected:
		Position pos;
		const Player rootPlayer;
};

class BaseNode {
	public:
		BaseNode(BaseNode* parent, Position& pos) : parent(parent), pos(pos), rootPlayer(pos.getTurn()) {} // CHECK learn about initialiser list
		Move getInEdge() const;
		Position& getPos();

	protected:
		Move inEdge;
		BaseNode* parent;
		std::vector<std::unique_ptr<BaseNode>> children; // CHECK unique? or shared?
		Position& pos;
		const Player rootPlayer;

		virtual void addChild(Move move) = 0;
};

#endif