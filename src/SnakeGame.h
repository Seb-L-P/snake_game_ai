#pragma once
#include "Snake.h"
#include <random>
#include <vector>

enum Action { TURN_LEFT = -1, STRAIGHT = 0, TURN_RIGHT = 1 };

Direction turn(Direction current, Action action);

class SnakeGame {
public:
    SnakeGame(int width, int height);

    void reset();

    double step(Direction action);

    void render() const;

    int getScore() const { return score; }

    bool isGameOver() const { return gameOver; }

    std::vector<double> getState() const;

private:
    int width, height;     // grid size
    Snake snake;           // the snake instance
    Point food;            // location of the food
    int score;             // total points
    bool gameOver;         // whether the game has ended
    std::mt19937 rng;      // random number generator

    // Helper function to place food randomly
    void placeFood();
};