#pragma once

#include "IPersistable.h"

struct Point : IPersistable
{
    Point() = default;
    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& p) = default;
    virtual ~Point() = default;

    void Save(Saver& saver) const override;
    void Load(Loader& loader) override;

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