#ifndef ILOCATION_PROVIDER
#define ILOCATION_PROVIDER

#include <vector>

struct Point;
class Actor;

class ILocationProvider
{
public:
	virtual bool IsWall(const Point& p) const = 0;
	virtual bool CanWalk(const Point& p) const = 0;
	virtual bool IsInFov(const Point& p) const = 0;

	virtual std::vector<Actor*> GetActorsWithinRange(Point centre, int range, bool onlyInFov, bool skipDeadActors, bool skipPlayer) const = 0;
	virtual Actor* GetClosestMonster(const Point start, const float range) const = 0;

	virtual Point GetPlayerLocation() const = 0;
	virtual Actor* GetPlayer() const = 0;
	virtual std::vector<Actor*> GetActorsAt(const Point& p) const = 0;
	virtual std::vector<Actor*>& GetActors() = 0;
};

#endif // ILOCAITON_PROVIDER