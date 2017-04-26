#ifndef SHIELD_H
#define SHIELD_H

#include "sprite.h"
#include "player.h"

class Player;

class Shield : public Sprite {
public:
	Shield(Player* player);
	Shield(const Shield&) = delete;
	Shield& operator=(const Shield&) = delete;

	void update(Uint32 ticks);

	float getRadius() const { return radius; }
	void setRadius(float r) { radius = r; }
private:
	Player* player;
	float radius;
};

#endif
