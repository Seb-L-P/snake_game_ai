#include "SnakeGame.h"
#include "Brain.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    SnakeGame game(20, 10);
    Brain brain;  // create an AI brain
    Direction currentDir = RIGHT;

    double totalReward = 0.0;

    while (!game.isGameOver()) {
        // 1. Get current game state
        std::vector<double> state = game.getState();

        // 2. Ask the brain what to do
        Action action = brain.decide(state);

        // 3. Update direction and move
        currentDir = turn(currentDir, action);

        //4. step environment and get reward/penalty
        double reward = game.step(currentDir);
        totalReward += reward;

        // 5. Render
        system("clear");
        game.render();

        // 6. Slow down for visibility
        std::cout << "Reward: " << reward << " | Total: " << totalReward << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    
    std::cout << "Game Over! Final Score: " << game.getScore() << "\n";
    std::cout << "Total Reward: " << totalReward << "\n";
}
