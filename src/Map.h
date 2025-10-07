#ifndef MAP_H
#define MAP_H

struct Point;

struct Tile
{
    bool canWalk;
    Tile() : canWalk(true) {}
};

class Map
{
public:
    Map(int width, int height);
    virtual ~Map();

    void Init();
    bool IsWall(const Point& position) const;
    void Render() const;
private:
    Tile* tiles{ nullptr };
    int width;
    int height;

    void SetWall(const Point& p);
};
#endif // MAP_H