#include "multisprite.h"
#include "sludge.h"

class Pool : public MultiSprite {
public:
	Pool(const Vector2f&);
	Pool(const Pool&);
	~Pool();

	void draw() const;
	void update(Uint32 ticks);

	void setSpawning(bool s) { spawning = s; }
private:
	bool spawning;
	int spawnTimer;

	void spawnSludge();
};
