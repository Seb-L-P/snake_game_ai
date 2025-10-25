#pragma once

struct Point {
    int x; //horizontal position
    int y; //vertical position

    //Constructor - makes creation of point easy 
    // e.g. Point p(3, 5);
    Point(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

