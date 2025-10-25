#include "SnakeGame.h"
#include "Brain.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    Brain brain;

    const int episodes = 10000;
    const int renderEvery = 1000; // 👀 show every 20th game
    const int renderDelay = 25; // ms delay between frames when rendering

    for (int episode = 1; episode <= episodes; ++episode) {
        SnakeGame game(20, 10);
        Direction currentDir = RIGHT;
        double totalReward = 0.0;

        bool shouldRender = (episode == 1) || (episode % renderEvery == 0) || (episode == episodes);


        while (!game.isGameOver()) {
            std::vector<double> state = game.getState();
            Action action = brain.decide(state);
            currentDir = turn(currentDir, action);
            double reward = game.step(currentDir);
            std::vector<double> nextState = game.getState();

            brain.update(state, action, reward, nextState);
            totalReward += reward;

            // 👀 render only sometimes
            if (shouldRender) {
                system("clear");
                std::cout << "Episode " << episode
                          << " | Score: " << game.getScore()
                          << " | Total Reward: " << totalReward << "\n";
                game.render();
                std::this_thread::sleep_for(std::chrono::milliseconds(renderDelay));
            }
        }

        std::cout << "Episode " << episode
                  << " | Final Score: " << game.getScore()
                  << " | Total Reward: " << totalReward << "\n";
    }

    std::cout << "Training complete!\n";
}
