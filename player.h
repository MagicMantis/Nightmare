#ifndef PLAYER_H
#define PLAYER_H

#include "twoWayMultiSprite.h"
#include "sludge.h"
#include "shield.h"
#include <list>

class Sludge;
class Shield;

class Player : public TwoWayMultiSprite {
public:
	Player(const std::string&);
	Player(const Player&);
	Player& operator=(const Player&) = delete;

	virtual void draw() const;
	virtual void update(Uint32 ticks);

	void attach(Sludge* s);
	void detach(Sludge* s);
	void notifyObservers();

	void accelerate(float amount); 
	void decelerate(float amount); 
	void jump();
	void stop();
	void makeShield();
	void popShield();
private:
	bool onGround();
	float fear;
	int jumps;

	//shield
	int shieldCooldown;
	Shield* shield;

	std::list<Sludge*> observers;
};

#endif
