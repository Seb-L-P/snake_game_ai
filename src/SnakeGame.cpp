#include "SnakeGame.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

// Constructor
SnakeGame::SnakeGame(int w, int h)
    : width(w), height(h), snake(w / 2, h / 2), rng(std::random_device{}()) {
    reset();
}

// Reset the game to its initial state
void SnakeGame::reset() {
    snake = Snake(width / 2, height / 2);
    score = 0;
    gameOver = false;
    stepsSinceFood = 0;
    placeFood();
}

// Place new food randomly where the snake is not
void SnakeGame::placeFood() {
    std::uniform_int_distribution<int> distX(1, width - 2);
    std::uniform_int_distribution<int> distY(1, height - 2);
    while (true) {
        food = { distX(rng), distY(rng) };
        bool onSnake = false;
        for (const auto& p : snake.getBody())
            if (p == food)
                onSnake = true;
        if (!onSnake) break;
    }
}

// --- Core RL update step ---
double SnakeGame::step(Direction dir) {
    if (gameOver) return 0.0;

    Point oldHead = snake.getHead();
    double distBefore = std::abs(oldHead.x - food.x) + std::abs(oldHead.y - food.y);

    snake.setDirection(dir);
    snake.move();

    Point head = snake.getHead();
    double distAfter = std::abs(head.x - food.x) + std::abs(head.y - food.y);

    double reward = -0.03; // small time penalty per move
    stepsSinceFood++;

    // --- Wall collision ---
    if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
        gameOver = true;
        return -110.0; // heavy punishment for dying
    }

    // --- Self collision ---
    if (snake.isSelfCollision()) {
        gameOver = true;
        return -250.0;
    }

    // --- Eating food ---
    if (head == food) {
        snake.grow();
        score += 10;
        placeFood();
        stepsSinceFood = 0;
        return +40.0; // big reward
    }

    // --- Distance shaping reward ---
    if (distAfter < distBefore)
        reward += +0.2; // moved closer to food
    else if (distAfter > distBefore)
        reward += -0.2; // moved further from food

    // --- Stale/hunger limit ---
    int staleLimit = 100 + (int)snake.getBody().size() * 10;
    if (stepsSinceFood > staleLimit) {
        gameOver = true;
        reward -= 4.0; // penalty for going too long without food
    }

    return reward;
}

// --- Get the current game state as features for the AI ---
std::vector<double> SnakeGame::getState() const {
    std::vector<double> state;

    Point head = snake.getHead();
    Direction dir = snake.getDirection();

    // --- 1. Current direction (4 one-hot) ---
    state.push_back(dir == UP ? 1.0 : 0.0);
    state.push_back(dir == DOWN ? 1.0 : 0.0);
    state.push_back(dir == LEFT ? 1.0 : 0.0);
    state.push_back(dir == RIGHT ? 1.0 : 0.0);

    // --- 2. Relative food position (4 binary features) ---
    state.push_back((food.x < head.x) ? 1.0 : 0.0); // food left
    state.push_back((food.x > head.x) ? 1.0 : 0.0); // food right
    state.push_back((food.y < head.y) ? 1.0 : 0.0); // food above
    state.push_back((food.y > head.y) ? 1.0 : 0.0); // food below

    // --- 3. Danger sensors: front, left, right ---
    auto isDanger = [&](int dx, int dy) -> double {
        Point check = { head.x + dx, head.y + dy };

        // Wall danger
        if (check.x <= 0 || check.x >= width - 1 || check.y <= 0 || check.y >= height - 1)
            return 1.0;
        // Body danger
        for (const auto& p : snake.getBody())
            if (p == check)
                return 1.0;
        return 0.0;
    };

    if (dir == UP) {
        state.push_back(isDanger(0, -1));  // front
        state.push_back(isDanger(-1, 0));  // left
        state.push_back(isDanger(1, 0));   // right
    } else if (dir == DOWN) {
        state.push_back(isDanger(0, 1));
        state.push_back(isDanger(1, 0));
        state.push_back(isDanger(-1, 0));
    } else if (dir == LEFT) {
        state.push_back(isDanger(-1, 0));
        state.push_back(isDanger(0, 1));
        state.push_back(isDanger(0, -1));
    } else if (dir == RIGHT) {
        state.push_back(isDanger(1, 0));
        state.push_back(isDanger(0, -1));
        state.push_back(isDanger(0, 1));
    }

    return state;
}

// --- Render function for visualizing the grid ---
void SnakeGame::render() const {
    std::vector<std::string> grid(height, std::string(width, ' '));

    // Draw borders
    for (int x = 0; x < width; ++x) {
        grid[0][x] = '#';
        grid[height - 1][x] = '#';
    }
    for (int y = 0; y < height; ++y) {
        grid[y][0] = '#';
        grid[y][width - 1] = '#';
    }

    // Draw food
    grid[food.y][food.x] = '*';

    // Draw snake
    for (const auto& segment : snake.getBody())
        grid[segment.y][segment.x] = 'O';

    // Display grid
    for (const auto& row : grid)
        std::cout << row << "\n";
}
