#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <vector>
#include <memory>

#include "sicario.hpp"

struct SearchInfo {
	int depth = 0;

	bool operator!=(const SearchInfo& s) {
		return this->depth != s.depth;
	}
};

class BaseSearcher {
	public:
		BaseSearcher(const Position& pos) : pos(pos), rootPlayer(pos.getTurn()) {}
		virtual void search(const std::atomic_bool& searchTree, SicarioConfigs& sicarioConfigs) = 0;
		Position& getPos();

	protected:
		Position pos;
		const Player rootPlayer;
};

class BaseNode {
	public:
		BaseNode(BaseNode* parent, Position& pos, SearchInfo& searchInfo) :
				parent(parent), searchInfo(searchInfo), pos(pos), rootPlayer(pos.getTurn()) {} // CHECK learn about initialiser list
		Move getInEdge() const;
		Position& getPos();

	protected:
		int depth;
		Move inEdge;
		BaseNode* parent;
		SearchInfo& searchInfo;
		std::vector<std::unique_ptr<BaseNode>> children; // CHECK unique? or shared?
		Position& pos;
		const Player rootPlayer;

		virtual void addChild(Move move) = 0;
};

#endif