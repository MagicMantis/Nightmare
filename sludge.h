#ifndef SLUDGE_H
#define SLUDGE_H

#include "sprite.h"
#include "player.h"

class Player;

class Sludge : public Sprite {
public:
	Sludge();
	Sludge(const Vector2f&, float);
	Sludge(const Sludge&) = delete;
	Sludge& operator=(const Sludge&) = delete;
	~Sludge() { }
	void notify(const Vector2f& location);

	float getRadius() const { return radius; }
	int getState() const { return state; }
	void setState(int s) { state = s; }

	// void draw() const;
	void update(Uint32 ticks);
private:
	float radius;
	Player* player;
	float gravity, damping, maxSpeed;

	//state 0 = seek player, state 1 = attached to player
	int state;
	Vector2f playerPos;
	float xoffset, yoffset; //offset from player when attached
	float grip; //how long the sludge can hold on

	Vector2f bounce(float);
};

#endif