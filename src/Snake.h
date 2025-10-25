#pragma once
#include <deque>
#include "Point.h"

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    // constructor to create new snake at x,y
    Snake(int startX, int startY);

    void move();

    void grow();

    void setDirection(Direction dir);

    Direction getDirection() const { return dir; }

    const std::deque<Point>& getBody() const { return body; }

    Point getHead() const { return body.front(); }

    bool isSelfCollision() const;

private:
    std::deque<Point> body;
    Direction dir;
};