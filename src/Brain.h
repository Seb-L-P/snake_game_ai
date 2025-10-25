#pragma once
#include "SnakeGame.h"
#include <unordered_map>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <algorithm>

class Brain {
private:
    std::unordered_map<std::string, std::vector<double>> qTable; // Q[state][action]
    std::mt19937 rng;
    std::uniform_real_distribution<double> rand01;
    double learningRate;
    double discount;
    double epsilon;

public:
    Brain()
        : rng(std::random_device{}()), rand01(0.0, 1.0),
          learningRate(0.3), discount(0.95), epsilon(0.2) {}

    // Converts a vector<double> to a comma-separated string key
    std::string stateToKey(const std::vector<double>& state) const {
        std::ostringstream oss;
        for (double v : state) oss << v << ",";
        return oss.str();
    }

    Action decide(const std::vector<double>& state) {
        std::string key = stateToKey(state);

        // Ensure state exists in table
        if (qTable.find(key) == qTable.end())
            qTable[key] = std::vector<double>(3, 0.0); // 3 actions: left, straight, right

        // ε-greedy: choose random action or best known
        if (rand01(rng) < epsilon)
            return static_cast<Action>(rand() % 3 - 1); // -1, 0, or 1
        else {
            auto& qValues = qTable[key];
            int bestIdx = std::distance(qValues.begin(), std::max_element(qValues.begin(), qValues.end()));
            return static_cast<Action>(bestIdx - 1);
        }
    }

    void update(const std::vector<double>& state, Action action,
                double reward, const std::vector<double>& nextState) {
        std::string key = stateToKey(state);
        std::string nextKey = stateToKey(nextState);

        if (qTable.find(key) == qTable.end())
            qTable[key] = std::vector<double>(3, 0.0);
        if (qTable.find(nextKey) == qTable.end())
            qTable[nextKey] = std::vector<double>(3, 0.0);

        int a = action + 1; // shift action {-1,0,1} → {0,1,2}

        double oldValue = qTable[key][a];
        double nextMax = *std::max_element(qTable[nextKey].begin(), qTable[nextKey].end());

        // Q-learning update rule
        double newValue = oldValue + learningRate * (reward + discount * nextMax - oldValue);
        qTable[key][a] = newValue;
    }
};
