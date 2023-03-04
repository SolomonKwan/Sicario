#ifndef SEARCH_HPP
#define SEARCH_HPP

#include <vector>
#include <memory>

#include "sicario.hpp"

struct SearchInfo {
	public:
		SearchInfo();
		bool getHasChanged();
		void setHasChanged(bool hasChanged);
		int getDepth();
		void setDepth(int depth);

	private:
		bool hasChanged;
		int depth;
};

class BaseSearcher {
	public:
		BaseSearcher(const Position& pos, const std::atomic_bool& searchTree, const SicarioConfigs& sicarioConfigs);
		virtual void search() = 0;
		Position& getPos();

	protected:
		Position pos;
		const Player rootPlayer;
		const std::atomic_bool& searchTree;
		const SicarioConfigs& sicarioConfigs;
};

class BaseNode {
	public:
		BaseNode(BaseNode* parent, Move inEdge, Position& pos, SearchInfo& searchInfo);
		Move getInEdge() const;
		Position& getPos();

	protected:
		BaseNode* parent;
		Move inEdge;
		Position& pos;
		const Player rootPlayer;
		SearchInfo& searchInfo;
		std::vector<std::unique_ptr<BaseNode>> children;
		int depth;

		virtual void addChild(Move move) = 0;
};

#endif