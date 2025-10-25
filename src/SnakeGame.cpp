#include "SnakeGame.h"
#include <iostream>
#include <ctime>

Direction turn(Direction current, Action action) {
    // Turn relative to the current direction
    if (action == STRAIGHT) return current;

    if (current == UP) {
        return (action == TURN_LEFT) ? LEFT : RIGHT;
    } else if (current == DOWN) {
        return (action == TURN_LEFT) ? RIGHT : LEFT;
    } else if (current == LEFT) {
        return (action == TURN_LEFT) ? DOWN : UP;
    } else { // RIGHT
        return (action == TURN_LEFT) ? UP : DOWN;
    }
}


//set up game world
SnakeGame::SnakeGame(int width, int height)
    : width(width), height(height), snake(width / 2, height / 2), rng(std::random_device{}()) {
    reset(); // start a new game
}

//resets shi back to beginning
void SnakeGame::reset() {
    snake = Snake(width / 2, height / 2); // reset snake to middle
    score = 0;
    gameOver = false;
    placeFood();
}

// Places food at a random location inside the boundaries
void SnakeGame::placeFood() {
    std::uniform_int_distribution<int> distX(1, width - 2);
    std::uniform_int_distribution<int> distY(1, height - 2);
    food = { distX(rng), distY(rng) };
}

// Advances the game by one step (move + logic)
double SnakeGame::step(Direction action) {
    if (gameOver) return -1.0;

    snake.setDirection(action);
    snake.move();

    Point head = snake.getHead();

    // Check for wall collision
    if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
        gameOver = true;
        return -10.0; //big negative reward due to hitting wall
    }

    // Check for self collision
    if (snake.isSelfCollision()) {
        gameOver = true;
        return -10.0; //big negative reward due to hitting itself
    }

    // Check if the snake eats the food
    if (head == food) {
        snake.grow();
        score += 10;
        placeFood();
        return +10.0; //reward for eating food
    }

    return -0.1; //small penalty for slow food collection
}

// Renders the game to the console
void SnakeGame::render() const {
    // Create a blank grid
    std::vector<std::string> grid(height, std::string(width, ' '));

    // Draw walls
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
    for (auto& segment : snake.getBody()) {
        grid[segment.y][segment.x] = 'O';
    }

    // Mark the head differently
    Point head = snake.getHead();
    grid[head.y][head.x] = '>';

    // Print to console
    for (const auto& row : grid) {
        std::cout << row << "\n";
    }
    std::cout << "Score: " << score << "\n\n";
}

std::vector<double> SnakeGame::getState() const {
    std::vector<double> state;

    Point head = snake.getHead();

    // --- 1. Current direction ---
    Direction dir = snake.getDirection();
    state.push_back(dir == UP ? 1.0 : 0.0);
    state.push_back(dir == DOWN ? 1.0 : 0.0);
    state.push_back(dir == LEFT ? 1.0 : 0.0);
    state.push_back(dir == RIGHT ? 1.0 : 0.0);

    // --- 2. Relative food position ---
    state.push_back((food.x < head.x) ? 1.0 : 0.0); // food left
    state.push_back((food.x > head.x) ? 1.0 : 0.0); // food right
    state.push_back((food.y < head.y) ? 1.0 : 0.0); // food above
    state.push_back((food.y > head.y) ? 1.0 : 0.0); // food below

    // --- 3. Danger detection (front, left, right) ---
    // We’ll check what’s directly adjacent to the head.
    auto isDanger = [&](int dx, int dy) -> double {
        Point check = { head.x + dx, head.y + dy };
        // Wall collision?
        if (check.x <= 0 || check.x >= width - 1 || check.y <= 0 || check.y >= height - 1)
            return 1.0;
        // Body collision?
        for (const auto& p : snake.getBody())
            if (p == check)
                return 1.0;
        return 0.0;
    };

    // Depending on current direction, define where "front", "left", and "right" are
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
