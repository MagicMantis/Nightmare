#include "shield.h"
#include "objectManager.h"

Shield::Shield(Player* p) : 
	Sprite("shield", Vector2f(p->getX()+16, p->getY()+32), .01),
	player(p),
	radius(Gamedata::getInstance().getXmlFloat("shield/radius")*getScale())
{ }

void Shield::update(Uint32 ticks) {
	setScale(getScale() + (0.005 * ticks));
	setX(player->getX()+32-getRadius());
	setY(player->getY()+32-getRadius());
	radius = Gamedata::getInstance().getXmlFloat("shield/radius")*getScale();
	
	//
	for (int xx = -4; xx <= 4; xx++) {
		for (int yy = -4; yy <= 4; yy++) {
			auto* objs = ObjectManager::getInstance().getObjectsInGrid(getGridX()+xx, getGridY()+yy);
			if (objs) {
				for (Collider* c : *objs) {
					if (c == this) continue;
					Sludge* sludge = dynamic_cast<Sludge*>(c);
					if (sludge) {
						float dist = getDistance(sludge);
						if (dist < sludge->getRadius()+getRadius()) {
							sludge->explode();
							//sludge->blast(Vector2f(player->getX()+16, player->getY()+32));
						}
					}
				}
			}
		}
	}

	if (getScale() >= 1.0) player->popShield();
}