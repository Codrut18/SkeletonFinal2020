#pragma once

class Heuristics
{
public:

    template <class State_t>
	static size_t GetManhattanDistance(const State_t& state)
	{
        auto data = state.GetData();
        size_t totalDistance = 0u;
        for (auto it = data.begin(); it != data.end(); it++)
        {
            if (*it == 0)
                continue;
            auto&& [line, column] = State_t::GetPosition2D(std::distance(data.begin(), it));
            totalDistance += std::abs((int)((*it - 1) / State_t::Dimension - line)) + std::abs((int)((*it - 1) % State_t::Dimension - column));
        }
        return totalDistance;
	}

    template <class State_t>
    static size_t GetGoalRow(const State_t& state, typename State_t::ElementType value)
    {
        const auto goalPosition = State_t::GetPosition2D(std::distance(state.GetData().begin(), std::find(state.GetData().begin(), state.GetData().end(), value)));
        return goalPosition.first;
    }

    template <class State_t>
    static size_t GetGoalColumn(const State_t& state, typename State_t::ElementType value)
    {
        const auto goalPosition = State_t::GetPosition2D(std::distance(state.GetData().begin(), std::find(state.GetData().begin(), state.GetData().end(), value)));
        return goalPosition.second;
    }

    template <class State_t>
    static size_t GetRowLinearConflict(const State_t& state, size_t row)
    {
        size_t conflictCount = 0;
        const auto& data = state.GetData();

        for (size_t col1 = 0; col1 < State_t::Dimension; ++col1)
        {
            if (data[row * State_t::Dimension + col1] != 0 &&
                GetGoalRow(state, data[row * State_t::Dimension + col1]) == row)
            {
                for (size_t col2 = col1 + 1; col2 < State_t::Dimension; ++col2)
                {
                    if (data[row * State_t::Dimension + col2] != 0 &&
                        GetGoalRow(state, data[row * State_t::Dimension + col2]) == row &&
                        data[row * State_t::Dimension + col1] > data[row * State_t::Dimension + col2])
                    {
                        conflictCount += 2;
                    }
                }
            }
        }

        return conflictCount;
    }

    template <class State_t>
    static size_t GetColumnLinearConflict(const State_t& state, size_t col)
    {
        size_t conflictCount = 0;
        const auto& data = state.GetData();

        for (size_t row1 = 0; row1 < State_t::Dimension; ++row1)
        {
            if (data[row1 * State_t::Dimension + col] != 0 &&
                GetGoalColumn(state, data[row1 * State_t::Dimension + col]) == col)
            {
                for (size_t row2 = row1 + 1; row2 < State_t::Dimension; ++row2)
                {
                    if (data[row2 * State_t::Dimension + col] != 0 &&
                        GetGoalColumn(state, data[row2 * State_t::Dimension + col]) == col &&
                        data[row1 * State_t::Dimension + col] > data[row2 * State_t::Dimension + col])
                    {
                        conflictCount += 2;
                    }
                }
            }
        }

        return conflictCount;
    }

    template <class State_t>
    static size_t GetLinearConflict(const State_t& state)
    {
        size_t conflictCount = 0;

        for (size_t row = 0; row < State_t::Dimension; ++row)
        {
            conflictCount += GetRowLinearConflict(state, row);
            conflictCount += GetColumnLinearConflict(state, row);
        }

        return conflictCount;
    }


    template <class State_t>
    static size_t GetManhattanDistanceWithLinearConflict(const State_t& state)
    {
        size_t manhattanDistance = GetManhattanDistance(state);
        size_t linearConflict = GetLinearConflict(state);
        return manhattanDistance + 2*linearConflict;
    }
};

