#pragma once

#include "types.h"
#include <array>
#include <optional>
#include <map>
#include <numeric>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <set>
#include <unordered_map>
#include <cmath>

template<size_t N = 3>
class State
{
public: // Types
    using Position2D = std::pair<size_t, size_t>;
    using ElementType = uint8_t;
	using Data = std::array<ElementType, N*N>;


private: // members
    Data m_data;

public:
    static const size_t Dimension = N;    

    State() = delete; // TODO
    State(Data data) : m_data{ std::move(data) } {};

    const Data& GetData() const
    {
        return m_data;
    }

    static const State GoalState()
    {
        // TODO: Refactor with STL        
        Data goalData;         
        std::iota(goalData.begin(), goalData.end(), 1);
        goalData.back() = 0;        
        return State(goalData);
    }

    bool IsGoalState() const
    {
        return m_data == GoalState().m_data;
    }

    bool IsValid() const
    {
        // TODO refactor with STL(the 3 fors)
		// no duplicates
		// pieces 1 - 8 
		// empty space 0 present

        //return std::is_permutation(m_data.begin(), m_data.end(), GoalState().GetData().begin());
        return std::ranges::is_permutation(m_data, GoalState().GetData());

        /*Data sortedData;
        
        sortedData = GetData();

        std::sort(sortedData.begin(), sortedData.end());

        Data validSortedData;
        std::iota(validSortedData.begin(), validSortedData.end(), 0);

        return sortedData == validSortedData;*/
    }

    size_t CountInversions(typename Data::const_iterator begin, typename Data::const_iterator end) const
    {
        size_t acc{ 0u };
        for (auto it = begin; it != end; ++it)
        {
            auto&& current = *it;
            if (current != 0)
                acc += std::count_if(it, end, [current](auto next) { return next != 0 && next < current; });
        }

        return acc;
    }

    bool IsSolvable() const
    {
		// TODO too big lambda

        const auto inversionsCount = CountInversions(m_data.begin(), m_data.end());
        const auto isInversionCountEven = inversionsCount % 2 == 0;
        const bool isNOdd = N % 2 == 1;
        const bool isBlankRowEven = GetBlankPosition2D().first % 2 == 0;

        return (isNOdd) ? isInversionCountEven :
            (isBlankRowEven) ? !isInversionCountEven :
            isInversionCountEven;
    }

    std::vector<std::pair<State, MoveDirection>> GetChildren() const
    {
        //TODO: Refactor this method + See Move method and refactor the Move method

        std::vector<std::pair<State, MoveDirection>> children;

        static Moves moves = { MoveDirection::UP, MoveDirection::DOWN, MoveDirection::LEFT, MoveDirection::RIGHT };

        for (auto direction : moves)
        {
            auto state = Move(direction);
            if (state) children.emplace_back(std::make_pair(*state, direction));
        }

        return children;
    }

    bool operator==(const State& other) const
    {
        return m_data == other.m_data;
    }

    bool operator>(const State& other) const
    {
        return GetManhattanDistance() > other.GetManhattanDistance();
    }

private: // methods

    size_t GetBlankPosition() const
    {
        const auto& it = std::ranges::find(m_data,0);

        if (it != m_data.end())
            return std::distance(m_data.begin(), it);

        throw std::runtime_error("Unexpected");
    }

public:
    static Position2D GetPosition2D(size_t position)
    {
        return { position / N, position % N };
    }

private:

    Position2D GetBlankPosition2D() const
    {
        return GetPosition2D(GetBlankPosition());
    }


    // TODO: Perform the move if possible and return the state. Returns std::nullopt otherwise.
    std::optional<State> Move(MoveDirection direction) const
    {
        static std::unordered_map<MoveDirection, std::function<std::optional<State>(const State&)>> moveAction = {
            {MoveDirection::LEFT,   std::mem_fn(&State::MoveLeft)},
            {MoveDirection::UP,     std::mem_fn(&State::MoveUp)},
            {MoveDirection::RIGHT,  std::mem_fn(&State::MoveRight)},
            {MoveDirection::DOWN,   std::mem_fn(&State::MoveDown)}
        };

        auto state = moveAction.find(direction);
        if(state == moveAction.end())
            throw std::runtime_error("Not implemented.");
        return state->second(*this);
    }

    static State SwapTiles(const State& state, size_t firstPos, size_t secondPos)
    {
        auto childData = state.GetData();
        std::swap(childData[firstPos], childData[secondPos]);
        return { std::move(childData) };
    }

    std::optional<State> MoveRight() const
    {
        if (GetBlankPosition2D().second == 0) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition - 1);
    }

    std::optional<State> MoveLeft() const
    {
        if (GetBlankPosition2D().second == N - 1) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition + 1);
    }

    std::optional<State> MoveDown() const
    {
        if (GetBlankPosition2D().first == 0) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition - N);
    }

    std::optional<State> MoveUp() const
    {
        if (GetBlankPosition2D().first == N - 1) return std::nullopt;

        auto blankPosition = GetBlankPosition();
        return SwapTiles(*this, blankPosition, blankPosition + N);
    }

public:
    size_t GetManhattanDistance() const
    {
        size_t totalDistance = 0u;
        for (auto it = m_data.begin(); it != m_data.end(); it++)
        {
            if (*it == 0)
                continue;
            auto&& [line,column] = GetPosition2D(std::distance(m_data.begin(), it));
            totalDistance += std::abs((int)((*it-1) / N - line)) + std::abs((int)((*it-1) % N - column));
        }
        return totalDistance;
    }

};

std::ostream& operator<< (std::ostream& os, MoveDirection dir)
{
    static const std::map<MoveDirection, const char*> namesMap
    {
        { MoveDirection::LEFT, "l" },
        { MoveDirection::RIGHT, "r" },
        { MoveDirection::UP, "u" },
        { MoveDirection::DOWN, "d" },
    };

    os << namesMap.at(dir);
    return os;
}

template <size_t N>
std::ostream& operator<< (std::ostream& os, const State<N>& state)
{
    os << std::endl;
    for (size_t index = 0; index < state.GetData().size(); ++index)
    {
        os << static_cast<unsigned int>(state.GetData()[index]) << ' ';

        if (index % State<>::Dimension == State<>::Dimension - 1) os << std::endl;
    }
    os << std::endl;

    return os;
}

using State3X3 = State<3>;
using State4X4 = State<4>;

namespace std {

    template<size_t N>
    struct hash<State<N>>
    {
        size_t operator() (const State<N>& state) const
        {
            static const std::hash<State<N>::ElementType> hasher;
            size_t seed = 0u;
            for (auto& elem : state.GetData())
            {
                seed ^= hasher(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            return seed;
        }
    };
}