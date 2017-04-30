#include "sludge.h"
#include "gamedata.h"
#include "objectManager.h"
#include "viewport.h"
#include <math.h>

Sludge::Sludge() : Sprite("sludge", Vector2f(
	Gamedata::getInstance().getRandInRange(0, 200),
	Gamedata::getInstance().getRandInRange(0, 200)),
	1.0),
	radius(Gamedata::getInstance().getXmlFloat("sludge/radius")),
	player(nullptr),
	gravity(Gamedata::getInstance().getXmlFloat("gravityConstant")),
	damping(Gamedata::getInstance().getXmlFloat("dampingConstant")),
	maxSpeed(Gamedata::getInstance().getXmlFloat("maxSpeed")),
	state(0),
	playerPos(),
	xoffset(0), yoffset(0), grip(0)
{ }

Sludge::Sludge(const Vector2f& pos, float r) : Sprite("sludge", pos, 
	r/Gamedata::getInstance().getXmlFloat("sludge/radius")),
	radius(r),
	player(nullptr),
	gravity(Gamedata::getInstance().getXmlFloat("gravityConstant")),
	damping(Gamedata::getInstance().getXmlFloat("dampingConstant")),
	maxSpeed(Gamedata::getInstance().getXmlFloat("maxSpeed")), 
	state(0),
	playerPos(),
	xoffset(0), yoffset(0), grip(0)
{ }

// void Sludge::draw() const {

// }

void Sludge::update(Uint32 ticks) {
	Collider::update(ticks);

	Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
	setPosition(getPosition() + incr);

	//free move
	if (state == 0) {
		//calculate change in velocity this tick
		Vector2f accel;

		//gravity
		float vspeed = getVelocityY();
		if (vspeed < 200.0) accel[1] += gravity;

		//floor bounce
		if (getY()-getFrameHeight()/2 > (Gamedata::getInstance().getXmlInt("world/ground"))) {
			float normalForce = (1.0)*pow((getY()-(getWorldHeight()-100)),2);
			accel[1] -= normalForce/5.0;
			setY(Gamedata::getInstance().getXmlInt("world/ground")+getFrameHeight()/2);
		}

		//bounce off other sludges
		accel = accel + bounce(gravity);

		//seek target
		if (!player) player = (Player *)ObjectManager::getInstance().getObject("player");
		Vector2f target = player->getPosition();
		//target[0] = Gamedata::getInstance().getMouseX() + Viewport::getInstance().getX();
		//target[1] = Gamedata::getInstance().getMouseY() + Viewport::getInstance().getY();
		target[0] += 16;
		target[1] += 32;
		float dist = getDistance(target);
		float xratio = (getX()-target[0]) / dist;
		float yratio = (getY()-target[1]) / dist;
		accel[0] -= Gamedata::getInstance().getXmlFloat("sludge/seekSpeed") * xratio * 2;
		accel[1] -= Gamedata::getInstance().getXmlFloat("sludge/seekSpeed") * yratio;

		if (dist < getRadius()) {
			player->attach(this);
			xoffset = getX()-target[0]+16;
			yoffset = getY()-target[1]+32;
			state = 1;
			grip = 3000;
		}

		//damping
		accel = accel - (damping * getVelocity());

		incr = accel * static_cast<float>(ticks) * .02;
		setVelocity(getVelocity() + incr);
		setVelocityX(Gamedata::clamp(getVelocityX(), -maxSpeed, maxSpeed));
		setVelocityY(Gamedata::clamp(getVelocityY(), -maxSpeed, maxSpeed));
	}
	else if (state == 1) {
		int diff = getDistance(playerPos);
		setX(playerPos[0]+xoffset);
		setY(playerPos[1]+yoffset);
		grip -= diff;

		if (grip <= 0) {
			player->detach(this);
			state = 0;
			grip = 0;
			xoffset = 0;
			yoffset = 0;
		}
	}
}

//Bounce off other sludges
Vector2f Sludge::bounce(float gravityConstant) {
	Vector2f accel;
	for (int xx = -1; xx <= 1; xx++) {
		for (int yy = -1; yy <= 1; yy++) {
			auto* objs = ObjectManager::getInstance().getObjectsInGrid(getGridX()+xx, getGridY()+yy);
			if (objs) {
				for (Collider* c : *objs) {
					if (c == this) continue;
					Sludge* sludge = dynamic_cast<Sludge*>(c);
					if (sludge) {
						float dist = getDistance(sludge);
						float xratio = (getX()-sludge->getX()) / dist;
						float yratio = (getY()-sludge->getY()) / dist;
						if (dist < radius) {
							float magnitude = gravityConstant / sqrt((radius - dist) / radius);
							accel[0] += xratio * magnitude;
							accel[1] += yratio * magnitude;
						}
					}
				}
			}
		}
	}
	return accel;
}

void Sludge::blast(const Vector2f& origin) {
	//detach from player
	player->detach(this);
	state = 0;
	grip = 0;
	xoffset = 0;
	yoffset = 0;

	//fly from blast origin
	float dist = getDistance(origin);
	float xratio = (getX()-origin[0]) / dist;
	float yratio = (getY()-origin[1]) / dist;
	setVelocity(Vector2f(xratio*1000, yratio*1000));
}

void Sludge::notify(const Vector2f& location) {
	playerPos = location;
}

void Sludge::reset() {
	player->detach(this);
	state = 0;
	grip = 0;
	xoffset = 0;
	yoffset = 0;
}