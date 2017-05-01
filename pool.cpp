#include "pool.h"
#include "objectManager.h"

Pool::Pool(const Vector2f& pos) : 
	MultiSprite("pool"), 
	spawning(true), 
	spawnTimer(0)
{ setPosition(pos); }

Pool::Pool(const Pool& p) :
	MultiSprite(p),
	spawning(p.spawning),
	spawnTimer(p.spawning)
{ }

Pool::~Pool() {
	
}

void Pool::draw() const {
	MultiSprite::draw();
}

void Pool::update(Uint32 ticks) {
	MultiSprite::update(ticks);

	if (spawning) {
		if (spawnTimer <= 0) {
			//spawnSludge();
			if (ObjectManager::getInstance().getFreeList().empty()) spawnTimer = 1200;
		}
	}

	if (spawnTimer > 0) spawnTimer -= ticks;
	if (spawnTimer < 0) spawnTimer = 0;
}

void Pool::spawnSludge() {
	if (ObjectManager::getInstance().getFreeList().empty()) {
		Vector2f spawnLoc = getPosition();
		spawnLoc[0] += (frameWidth / 2) + Gamedata::getInstance().getRandInRange(-30,30);
		spawnLoc[1] += (frameHeight / 2) + Gamedata::getInstance().getRandInRange(-20,0);
		Sludge* s = new Sludge(spawnLoc, Gamedata::getInstance().getXmlInt("sludge/radius"));
		ObjectManager::getInstance().addObject(s);
	}
	else {
		Sludge* s = ObjectManager::getInstance().getFreeObj();
		Vector2f spawnLoc = getPosition();
		spawnLoc[0] += (frameWidth / 2) + Gamedata::getInstance().getRandInRange(-30,30);
		spawnLoc[1] += (frameHeight / 2) + Gamedata::getInstance().getRandInRange(-20,0);
		s->setPosition(spawnLoc);
		s->setState(0);
		ObjectManager::getInstance().addObject(s);
	}
}