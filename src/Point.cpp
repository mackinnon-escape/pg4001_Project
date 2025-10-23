#include "Point.h"

#include <cmath>
#include "Serialise.h"

const Point Point::Zero{ 0,0 };

float Point::DistanceTo(const Point& target) const
{
    int dx = x - target.x;
    int dy = y - target.y;
    return sqrtf(static_cast<float>(dx * dx + dy * dy));
}

Point Point::operator+(const Point& rhs) const
{
    return { x + rhs.x, y + rhs.y };
}

Point Point::operator-(const Point& rhs) const
{
    return { x - rhs.x, y - rhs.y };
}

Point Point::operator/(const int rhs) const
{
    return { x / rhs, y / rhs };
}

Point Point::operator*(const int rhs) const
{
    return { x * rhs, y * rhs };
}

bool Point::operator==(const Point& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

void Point::Load(Loader& loader)
{
    x = loader.GetInt();
    y = loader.GetInt();
}

void Point::Save(Saver& saver) const
{
    saver.PutInt(x);
    saver.PutInt(y);
}