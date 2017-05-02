#ifndef OBJECTMANAGER__H
#define OBJECTMANAGER__H

#include "drawable.h"
#include "collider.h"
#include <vector>
#include <unordered_map>
#include <list>
#include "sludge.h"

/**
 * ObjectManager class: this class contains a vector of all game
 * objects and manages updating and rendering them each frame.
 */
class ObjectManager {
public:
	static ObjectManager& getInstance();
	static void reset(); //recreate object manager
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;
	~ObjectManager();


	void addObject(Drawable*, bool = false); //add an object 
	void removeObject(Drawable*); //remove an object from being updated and drawn
	void initObjects(); //generate all objects in the scene
	void updateObjects(Uint32 ticks); //update all objects
	void drawObjects() const; //draw all objects

	int getInstanceCount() const { return gameObjects.size(); }
	Drawable* getObject(int index) const;

	Drawable* getObject(const std::string&);
	std::list<Drawable*>* getObjectsOfType(const std::string&);

	void changeGrid(int, int, int, int, Collider*);
	std::list<Collider*>* getObjectsInGrid(int, int) const;

	std::list<Sludge*> getFreeList() const { return freeList; }
	Sludge* getFreeObj();

	void playerRespawn(Uint32 ticks);
	void storePlayer(Player *p) { playerStore = p; }
private:
	ObjectManager(int w, int h);
	static ObjectManager *objectManager;
	std::list<Drawable*> gameObjects;
	std::unordered_map<std::string, std::list<Drawable*>*> instanceSets;
	std::list<Drawable*> removeList;
	std::list<Sludge*> freeList;
	int gridXs, gridYs;
	std::list<Collider*> *grid;
	int gridWidth, gridHeight;
	Player *playerStore;
};

#endif