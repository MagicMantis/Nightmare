#ifndef PLAYER_H
#define PLAYER_H

#include "twoWayMultiSprite.h"
#include "sludge.h"
#include <list>

class Sludge;

class Player : public TwoWayMultiSprite {
public:
	Player(const std::string&);
	Player(const Player&);

	virtual void draw() const;
	virtual void update(Uint32 ticks);

	void attach(Sludge* s);
	void detach(Sludge* s);
	void notifyObservers();

	void accelerate(float amount); 
	void decelerate(float amount); 
	void jump();
	void stop();
private:
	bool onGround();
	float fear;
	int jumps;

	std::list<Sludge*> observers;
};

#endif
