#pragma once
#include "SnakeGame.h"
#include <vector>
#include <random>

class Brain {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

public:
    Brain()
        : rng(std::random_device{}()), dist(-1, 1) {} // initialize random generator

    Action decide(const std::vector<double>& state) {
        // For now: pick random action
        // Later: replace with neural network output
        return static_cast<Action>(dist(rng));
    }
};
