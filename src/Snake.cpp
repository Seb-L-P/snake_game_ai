#include "Snake.h"

Snake::Snake(int startX, int startY) {
    body.push_back({startX, startY});
    dir = RIGHT;
}

void Snake::move() {
    Point head = body.front();
    switch (dir) {
        case UP:    head.y--; break;
        case DOWN:  head.y++; break;
        case LEFT:  head.x--; break;
        case RIGHT: head.x++; break;
    }

    // Add new head to the front of the deque
    body.push_front(head);

    // Remove the last tail segment (unless growing)
    body.pop_back();

}

// Makes the snake longer by one segment
void Snake::grow() {
    // Duplicate the last segment to increase length
    Point tail = body.back();
    body.push_back(tail);
}

// Changes the snake's direction, but prevents 180° reversals
void Snake::setDirection(Direction newDir) {
    // Prevent turning directly into itself
    if ((dir == UP && newDir != DOWN) ||
        (dir == DOWN && newDir != UP) ||
        (dir == LEFT && newDir != RIGHT) ||
        (dir == RIGHT && newDir != LEFT)) {
        dir = newDir;
    }
}

// Checks if the head collides with the body
bool Snake::isSelfCollision() const {
    Point head = body.front();

    // Start from index 1 (skip head)
    for (size_t i = 1; i < body.size(); ++i) {
        if (body[i] == head)
            return true;
    }

    return false;
}