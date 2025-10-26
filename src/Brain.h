#pragma once
#include "SnakeGame.h"
#include <unordered_map>
#include <vector>
#include <random>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

class Brain {
private:
    std::unordered_map<std::string, std::vector<double>> qTable; // Q[state][action]
    std::mt19937 rng;
    std::uniform_real_distribution<double> rand01;
    double learningRate;
    double discount;
    double epsilon;
    int updateCount = 0;

public:
    Brain()
        : rng(std::random_device{}()), rand01(0.0, 1.0),
          learningRate(0.3),       // a bit faster to learn early
          discount(0.98),           // value long-term reward more
          epsilon(0.40)             // start with high exploration
    {}

    // Compress state to a robust string key with fixed precision
    std::string stateToKey(const std::vector<double>& state) const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1);
        for (double v : state) oss << v << ",";
        return oss.str();
    }

    // ε-greedy decision
    Action decide(const std::vector<double>& state) {
        std::string key = stateToKey(state);
        if (qTable.find(key) == qTable.end())
            qTable[key] = std::vector<double>(3, 0.2); // optimistic init (>0) helps exploration

        if (rand01(rng) < epsilon)
            return static_cast<Action>(rand() % 3 - 1); // explore
        else {
            auto& qValues = qTable[key];
            int bestIdx = std::distance(qValues.begin(),
                         std::max_element(qValues.begin(), qValues.end()));
            return static_cast<Action>(bestIdx - 1); // exploit
        }
    }

    // Q-learning update
    void update(const std::vector<double>& state, Action action,
                double reward, const std::vector<double>& nextState) {
        std::string key = stateToKey(state);
        std::string nextKey = stateToKey(nextState);

        if (qTable.find(key) == qTable.end())     qTable[key]     = std::vector<double>(3, 0.2);
        if (qTable.find(nextKey) == qTable.end()) qTable[nextKey] = std::vector<double>(3, 0.2);

        int a = action + 1; // {-1,0,1} → {0,1,2}
        double oldValue = qTable[key][a];
        double nextMax  = *std::max_element(qTable[nextKey].begin(), qTable[nextKey].end());

        // Adaptive LR: cool down gradually as we gather more experience
        updateCount++;
        double cool = 1.0 / (1.0 + (updateCount / 75000.0)); // very gentle cooling
        double lr   = std::max(0.05, learningRate * cool);

        double target = reward + discount * nextMax;
        qTable[key][a] = oldValue + lr * (target - oldValue);
    }

    // Epsilon controls
    void setEpsilon(double e) { epsilon = e; }
    double getEpsilon() const { return epsilon; }
};
