#include "pool.h"

Pool::Pool(const Vector2f& pos) : 
	MultiSprite("pool"), 
	spawning(false), 
	spawnTimer(0),
	sludgeList(),
	freeList()
{ setPosition(pos); }

Pool::Pool(const Pool& p) :
	MultiSprite(p),
	spawning(p.spawning),
	spawnTimer(p.spawning),
	sludgeList(p.sludgeList),
	freeList(p.freeList)
{ }

void Pool::draw() {
	MultiSprite::draw();
	for (Sludge* s : sludgeList) {
		s->draw();
	}
}

void Pool::update(Uint32 ticks) {
	if (spawning) {
		if (spawnTimer <= 0) {
			spawnSludge();
		}
	}

	if (spawnTimer > 0) spawnTimer -= ticks;
	if (spawnTimer < 0) spawnTimer = 0;

	for (Sludge* s : sludgeList) {
		s->update(ticks);
	}
}

void Pool::spawnSludge() {
	if (freeList.empty()) {
		Sludge* s = new Sludge(getPosition(), Gamedata::getInstance().getXmlInt("sludge/radius"));
		sludgeList.push_back(s);
	}
	else {
		Sludge* s = freeList.front();
		freeList.pop_front();
		s->setPosition(getPosition());
		s->setState(0);
		sludgeList.push_back(s);
	}
}