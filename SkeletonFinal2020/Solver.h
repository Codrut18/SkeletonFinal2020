#pragma once

#include "State.h"
#include "io.h"
#include "Heuristics.h"

#include <iostream>
#include <set>
#include <unordered_set>
#include <queue>

class Solver
{
public:
    template <class State_t>
    static Moves SolveBFS(const State_t& initialState)
    {
        std::cout << "Solving..." << std::endl << initialState;

        Validate(initialState);
        if (initialState.IsGoalState()) return {}; // no moves required. Initial state is goal state.

        using Node = std::pair<State_t, Moves>;

        /*auto nodeGreaterComparer = [](const Node& first, const Node& second)
        {
            return Heuristics::GetManhattanDistance(first.first) + first.second.size() > Heuristics::GetManhattanDistance(second.first) + second.second.size();
        };*/

        auto nodeGreaterComparer = [](const Node& first, const Node& second)
        {
            return Heuristics::GetManhattanDistanceWithLinearConflict(first.first) + first.second.size() > Heuristics::GetManhattanDistanceWithLinearConflict(second.first) + second.second.size();
        };

        /*auto nodeGreaterComparer = [](const Node& first, const Node& second)
        {
            return Heuristics::GetManhattanDistanceWithLinearConflict(first.first) > Heuristics::GetManhattanDistanceWithLinearConflict(second.first);
        };*/

		//TODO: define OPEN SET correctly
		std::priority_queue<Node, std::vector<Node>, decltype(nodeGreaterComparer)> openSet(nodeGreaterComparer);
        openSet.push({ initialState, {} });

        auto stateCompare = [](const State_t& first, const State_t& second) -> bool
        {
            return first.GetData() < second.GetData();
        };
		
        std::unordered_set<State_t> closedSet;

        while (!openSet.empty())
        {
			// TODO: Do it nicer , who is first? second? structure binding
            auto currentNode = openSet.top();            
            auto&& [currentState, currentMoves] = currentNode;
            openSet.pop();

            // some logging
            static size_t maxDepth = 0;
            if (currentMoves.size() > maxDepth)
            {
                maxDepth = currentMoves.size();
                std::cout << "Max Depth: " << maxDepth << std::endl;
            }
            // end logging

            if (currentState.IsGoalState())
            {
                std::cout << "Visited: " << closedSet.size() << std::endl;                
                return currentMoves; // leaving cycle statement
            }

            closedSet.insert(currentState);

            for (auto&& childMovePair : currentState.GetChildren())
            {
				//TODO: Do it nicer - who is first? second? 
                auto&& [childState, move] = childMovePair;

				if(!closedSet.contains( childState)) // TODO: use C++20
                {
                    Moves childMoves = currentMoves;
                    childMoves.push_back(move);
                    openSet.push({ std::move(childState), std::move(childMoves) });
                }
            }
        }

        throw std::runtime_error("Couldn't solve");
    }

private:
    template <class State_t>
    static void Validate(const State_t& state)
    {
        if (!state.IsValid())
        {
            throw std::runtime_error("state not valid");
        }
        if (!state.IsSolvable())
        {
            throw std::runtime_error("state not solvable");
        }

		return;
    }
};
