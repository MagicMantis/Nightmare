#include <random>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "objectManager.h"
#include "sludge.h"
#include "smog.h"
#include "player.h"
#include "rain.h"
#include "pool.h"
#include "gamedata.h"
#include "viewport.h"

ObjectManager& ObjectManager::getInstance() {
  int gridWidth = Gamedata::getInstance().getXmlInt("grid/width");
  int gridHeight = Gamedata::getInstance().getXmlInt("grid/height");
  static ObjectManager objectManager(gridWidth, gridHeight);
  return objectManager;
}

void ObjectManager::initObjects() {
	//add pool
	Vector2f loc;
	loc[0] = 1600;
	loc[1] = 420;
	addObject( new Pool(loc) );
	loc[0] = 2300;
	addObject( new Pool(loc) );
	
	//add player
	addObject( new Player("player") );

	std::cerr << " fine to here\n ";

	//generate rain effects
	int rainCount = Gamedata::getInstance().getXmlInt("rain/count");
	float rainSpeed = Gamedata::getInstance().getXmlFloat("rain/speed");
	int w = Gamedata::getInstance().getXmlInt("view/width");
	int h = Gamedata::getInstance().getXmlInt("view/height");
	std::vector<Rain*> *rain_vec = new std::vector<Rain*>();
	rain_vec->reserve(10);
	for (int i = 0; i < rainCount; i++) {
		int len = rand()%4+3;
		Rain* r = new Rain((float)i*((float)w/(float)rainCount), (rand()%h), len, h*(.9)+h*(.1*len/6), rainSpeed);
		std::cerr << i << std::endl;
		rain_vec->push_back(r);
	}
	std::cerr << " fine to here\n ";
	for (Rain* r : *rain_vec) { 
		if (r->getLength() <= 3) addObject(r);
	}

	std::cerr << " fine to here\n ";
	//generate sludge balls
	int sludgeCount = Gamedata::getInstance().getXmlInt("sludgeCount")/2;
  	float u = Gamedata::getInstance().getXmlFloat("sludge/radius"); //Mean size
  	float d = Gamedata::getInstance().getXmlFloat("sludge/deviation"); //Std deviation
	
	std::random_device rd;
	std::mt19937 mt(rd());
	std::normal_distribution<float>dist(u,d);

	std::vector<Sludge*> tempVec;
	for (int i = 0; i < sludgeCount; i++) {
		Vector2f pos;
		pos[0] = Gamedata::getInstance().getRandInRange(-200, 0);
		pos[1] = Gamedata::getInstance().getRandInRange(200, 400);
		tempVec.push_back( new Sludge(pos, dist(mt)) );
	}
	for (int i = 0; i < sludgeCount; i++) {
		Vector2f pos;
		int width = Gamedata::getInstance().getXmlInt("world/width");
		pos[0] = Gamedata::getInstance().getRandInRange(width, width+200);
		pos[1] = Gamedata::getInstance().getRandInRange(200, 400);
		tempVec.push_back( new Sludge(pos, dist(mt)) );
	}

	sort(tempVec.begin(), tempVec.end(), [](const Sludge* const a, const Sludge* const b) 
		{ return a->getRadius() < b->getRadius(); });
	for (Sludge* s : tempVec) addObject( s );

	//finish effects
	for (Rain* r : *rain_vec) { 
		if (r->getLength() > 3) addObject(r);
	}
	for (int i = 0; i < 20; i++) {
		addObject( new Smog() );
	}

	rain_vec->clear();
	delete rain_vec;
}

void ObjectManager::addObject(Drawable* obj, bool front) {
	if (front) gameObjects.push_front(obj);
	else gameObjects.push_back(obj);
	auto search = instanceSets.find(obj->getName());
    if(search != instanceSets.end()) {
        search->second->push_back(obj);
    }
    else {
        instanceSets[obj->getName()] = new std::list<Drawable*>();
        instanceSets[obj->getName()]->push_back(obj);
    }
    Collider* collider = dynamic_cast<Collider*>(obj);
    if (collider) {
    	int x = (int) collider->getX() / gridWidth;
    	int y = (int) collider->getY() / gridHeight;
    	changeGrid(-1,-1,x,y,collider);
    }
}

void ObjectManager::removeObject(Drawable* obj) {
	removeList.push_back(obj);
}

Drawable* ObjectManager::getObject(int index) const {
	auto it = gameObjects.begin();
	while (index-- > 0) it++;
	return *it;
}

void ObjectManager::changeGrid(int x1, int y1, int x2, int y2, Collider* collider) {
	if (x1 >= 0 && x1 < gridXs && y1 >= 0 && y1 < gridYs)
		grid[x1*gridYs+y1].remove_if([collider](Collider* c){ return c->getID() == collider->getID();});
	if (x2 >= 0 && x2 < gridXs && y2 >= 0 && y2 < gridYs)
		grid[x2*gridYs+y2].push_back(collider);
}

std::list<Collider*>* ObjectManager::getObjectsInGrid(int x, int y) const {
	if (x < 0 || x >= gridXs || y < 0 || y >= gridYs) return nullptr;
	return &grid[x*gridYs+y];
}

//update all game objects
void ObjectManager::updateObjects(Uint32 ticks) {
	for (Drawable* d : gameObjects) {
		d->update(ticks);
	}
	playerRespawn(ticks);
	while (removeList.size() > 0) {	
		Collider *obj = dynamic_cast<Collider*>(removeList.front());
		gameObjects.remove(obj);
		instanceSets[obj->getName()]->remove(obj);
		if (obj && obj->getGridX() >= 0 && obj->getGridX() < gridXs && obj->getGridY() >= 0 && obj->getGridY() < gridYs)
			grid[(int) (obj->getGridX()*gridYs+obj->getGridY())].remove(obj);
		removeList.pop_front();
		Sludge *s = dynamic_cast<Sludge*>(obj);
		Player *p = dynamic_cast<Player*>(obj);
		if (s) {
			s->reset();
			freeList.push_back(s);	
		}
		else if (p) {
			playerStore = p;
		}
		//else delete obj;
	}
}

//draw all objects
void ObjectManager::drawObjects() const {
	for ( Drawable* d : gameObjects) {
		d->draw();
	}
}

ObjectManager::~ObjectManager() {
	for (auto& it : gameObjects) delete it;
	for (auto& it : instanceSets) {
		delete it.second;
	}
}

void ObjectManager::resetObjects() {
	for (auto& it : gameObjects) delete it;
	gameObjects.clear();
	for (auto& it : instanceSets) {
	 	it.second->clear();
	  	delete it.second;
	}
	for (int i = 0; i < gridXs; i++) {
		for (int j = 0; j < gridYs; j++) {
			grid[i*gridYs+j].clear();
		}
	}
	for (auto& it : freeList) delete it;
	freeList.clear();
	removeList.clear();
}

Drawable* ObjectManager::getObject(const std::string& type) {
	if ((*instanceSets[type]).empty()) return nullptr;
	return (*instanceSets[type]).front();
}

std::list<Drawable*>* ObjectManager::getObjectsOfType(const std::string& type) {
	return instanceSets[type];
}

Sludge* ObjectManager::getFreeObj() {
	Sludge *s = freeList.front();
	freeList.pop_front();
	return s;
}

void ObjectManager::playerRespawn(Uint32 ticks) {
	static int respawnTimer = Gamedata::getInstance().getXmlInt("player/respawn");
	if (playerStore) {
		respawnTimer -= ticks;
		if (respawnTimer <= 0) {
			respawnTimer = Gamedata::getInstance().getXmlInt("player/respawn");
			addObject(playerStore, true);
			Viewport::getInstance().setObjectToTrack(playerStore);
			playerStore = nullptr;
		}
	}

}

ObjectManager::ObjectManager(int w, int h) : gameObjects(), instanceSets(), removeList(), freeList(),
	gridXs(Gamedata::getInstance().getXmlInt("world/width") / w + 1), 
	gridYs(Gamedata::getInstance().getXmlInt("world/height") / h + 1),
	grid(new std::list<Collider*>[gridXs*gridYs]), gridWidth(w), gridHeight(h), 
	playerStore(nullptr) 
{ }