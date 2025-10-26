#include "SnakeGame.h"
#include "Brain.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <numeric>
#include <cmath>

// Local helper: convert relative Action into absolute Direction
static Direction turn(Direction current, Action action) {
    if (action == TURN_LEFT) {
        if (current == UP)    return LEFT;
        if (current == DOWN)  return RIGHT;
        if (current == LEFT)  return DOWN;
        if (current == RIGHT) return UP;
    } else if (action == TURN_RIGHT) {
        if (current == UP)    return RIGHT;
        if (current == DOWN)  return LEFT;
        if (current == LEFT)  return UP;
        if (current == RIGHT) return DOWN;
    }
    return current; // STRAIGHT
}

int main() {
    Brain brain;

    const int episodes    = 100000; // run a lot; learning is single-run only
    const int renderEvery = 10000;   // visual snapshot cadence
    const int renderDelay = 25;    // ms between frames when rendering
    const int width = 20, height = 10;

    // Epsilon schedule: keep exploration high for long, then taper very low
    const double maxEps = 0.30;
    const double minEps = 0.0001;
    const double earlyUntil = 6000.0;          // stay exploratory longer
    const double decayEarly = 0.00035;         // gentle early decay
    const double decayLate  = 0.0016;          // faster late decay

    // Rolling stats
    std::vector<double> recentRewards; recentRewards.reserve(200);
    std::vector<int>    recentScores;  recentScores.reserve(200);
    const size_t window = 100;

    for (int episode = 1; episode <= episodes; ++episode) {
        SnakeGame game(width, height);
        Direction currentDir = RIGHT;
        double totalReward = 0.0;

        // Two-phase epsilon decay (episode-based)
        double rate = (episode < earlyUntil) ? decayEarly : decayLate;
        double eps = minEps + (maxEps - minEps) * std::exp(-rate * episode);
        brain.setEpsilon(eps);

        bool shouldRender = (episode == 1) || (episode % renderEvery == 0) || (episode == episodes);

        // Guard against endless meandering:
        const int maxSteps = 800 + game.getScore() * 40; // more leniency as snake grows
        int steps = 0;

        while (!game.isGameOver() && steps < maxSteps) {
            std::vector<double> state = game.getState();
            Action action = brain.decide(state);
            currentDir = turn(currentDir, action);

            double reward = game.step(currentDir);  // shaped reward (see step() tweak below)
            std::vector<double> nextState = game.getState();

            brain.update(state, action, reward, nextState);
            totalReward += reward;
            steps++;

            if (shouldRender) {
                system("clear");
                std::cout << "Episode " << episode
                          << " | Score: " << game.getScore()
                          << " | Reward: " << totalReward
                          << " | Steps: " << steps
                          << " | Epsilon: " << eps << "\n";
                game.render();
                if (renderDelay > 0) std::this_thread::sleep_for(std::chrono::milliseconds(renderDelay));
            }
        }

        // Track stats
        recentRewards.push_back(totalReward);
        recentScores.push_back(game.getScore());
        if (recentRewards.size() > window) recentRewards.erase(recentRewards.begin());
        if (recentScores.size()  > window) recentScores.erase(recentScores.begin());

        double avgRew = std::accumulate(recentRewards.begin(), recentRewards.end(), 0.0) / recentRewards.size();
        double avgSco = std::accumulate(recentScores.begin(),  recentScores.end(),  0.0) / std::max<size_t>(1, recentScores.size());

        if (episode % 100 == 0 || shouldRender) {
            std::cout << "Episode " << episode
                      << " | Final Score: " << game.getScore()
                      << " | AvgReward(" << window << "): " << avgRew
                      << " | AvgScore("  << window << "): " << avgSco
                      << " | Epsilon: "  << eps
                      << " | Steps: "    << steps
                      << "\n";
        }
    }

    std::cout << "✅ Training complete (single run).\n";
    return 0;
}
