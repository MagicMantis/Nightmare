#include "player.h"
#include "gamedata.h"
#include "viewport.h"
#include "objectManager.h"
#include "sludge.h"
#include <algorithm>

Player::Player(const std::string& name) : 
	TwoWayMultiSprite(name), 
	fear(0.0),
	jumps(1),
	shieldCooldown(0),
	shield(nullptr),
	godMode(false),
	observers()
{ }

Player::Player(const Player& p) : 
	TwoWayMultiSprite(p.getName()),
	fear(p.fear),
	jumps(p.jumps),
	shieldCooldown(p.shieldCooldown),
	shield(p.shield),
	godMode(false),
	observers(p.observers)
{ }

// helper function
float clamp(float val, float low, float high) {
  if (val < low) val = low; 
  if (val > high) val = high;
  return val;
}

void Player::update(Uint32 ticks) {
	Collider::update(ticks);

	if (getVelocityX() != 0) TwoWayMultiSprite::update(ticks);
	Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
	setPosition(getPosition() + incr);

	//gravity
	if (!onGround()) {
		float gravity = Gamedata::getInstance().getXmlFloat("gravityConstant") 
			* static_cast<float>(ticks) * 0.04;
		setVelocityY(getVelocityY() + gravity);
	}

	//acceleration
	float accel = Gamedata::getInstance().getXmlFloat("player/acceleration")
		* static_cast<float>(ticks) * 0.04;
	float decel = Gamedata::getInstance().getXmlFloat("player/deceleration")
		* static_cast<float>(ticks) * 0.04;
	if (Gamedata::getInstance().getRightKey()) accelerate(accel);
	else if (getVelocityX() > 0) decelerate(decel);
	if (Gamedata::getInstance().getLeftKey()) accelerate(-accel);
	else if (getVelocityX() < 0) decelerate(decel);

	if (shield) shield->update(ticks);
	else {
		shieldCooldown -= ticks;
		if (shieldCooldown < 0) shieldCooldown = 0;
	}

	notifyObservers();
	
	fear += 0.005 * observers.size();
	fear -= .01;
	if (shield) fear -= .3;
	fear = Gamedata::clamp(fear, 0.0, 10.0);
	Viewport::getInstance().setJitter(fear);
	Viewport::getInstance().setFade(fear);
	if (fear >= 10.0) {
		Viewport::getInstance().setFade(0);
		Viewport::getInstance().setJitter(0);
		Viewport::getInstance().setObjectToTrack(nullptr);
		explode();
		fear = 0.0;
		ObjectManager::getInstance().storePlayer(this);
	}
}

void Player::draw() const {
	TwoWayMultiSprite::draw();
	if (shield) shield->draw();
}

void Player::attach(Sludge* s) {
	if (s->getState() == 0)
		observers.push_back(s);
}

void Player::detach(Sludge* s) {
	if (s->getState() == 1)
		observers.remove(s);
}

void Player::notifyObservers() {
	for (Sludge* s : observers) {
		s->notify(getPosition());
	}
}

void Player::accelerate(float amount) {
	float mod = ((10.0 - fear) / 10.0);
	setVelocityX(getVelocityX() + (amount*mod));
	float maxSpeed = Gamedata::getInstance().getXmlFloat("player/maxSpeed") * mod;
	setVelocityX(Gamedata::clamp(getVelocityX(), -maxSpeed, maxSpeed));
}

void Player::decelerate(float amount) {
	float sign = (getVelocityX() >= 0 ? -1.0 : 1.0);
	if (abs(getVelocityX()) < amount) {
		currentFrame = 0;
		setVelocityX(0.0);
	}
	else setVelocityX(getVelocityX() + (amount * sign));
}

void Player::jump() {
	if (!onGround() && jumps == 0) return;
	if (!onGround()) jumps--;
	else jumps = 1;
	float jumpPower = Gamedata::getInstance().getXmlFloat("player/jumpPower");
	float mod = ((10.0 - fear) / 10.0);
	setVelocityY(-(jumpPower*mod));
}

void Player::stop() {
	Vector2f stopSpeed;
	setVelocity(stopSpeed);
}

bool Player::onGround() {
	float groundLocation = Gamedata::getInstance().getXmlFloat("world/ground");
	if (getY()+frameHeight/2 > groundLocation) {
		setVelocityY(0);
		setY(groundLocation-frameHeight/2);
	}
	if (getY()+frameHeight/2 == groundLocation) return true;
	return false;
}

bool Player::makeShield() {
	if (!shield && shieldCooldown == 0) {
		shield = new Shield(this);
		shieldCooldown = Gamedata::getInstance().getXmlInt("player/shieldCooldown");
		return true;
	}
	return false;
}

void Player::popShield() {
	if (shield) { 
		delete shield;
		shield = nullptr;
	}
}