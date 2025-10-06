#pragma once

struct Point
{
    Point() = default;
    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& p) = default;
    virtual ~Point() = default;

    float DistanceTo(const Point& target) const;

    Point operator+(const Point& rhs) const;
    Point operator-(const Point& rhs) const;
    Point operator/(const int rhs) const;
    Point operator*(const int rhs) const;
    Point& operator=(const Point& rhs) = default;
    bool operator==(const Point& rhs) const;
    bool operator!=(const Point& rhs) const { return !(*this == rhs); }

    int x{ 0 };
    int y{ 0 };

    const static Point Zero;
};