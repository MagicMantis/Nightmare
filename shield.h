#include "sprite.h"

class Shield : public Sprite {
public:
	Shield(const Vector2f&);

	void draw() const;
	void update(Uint32 ticks);

	float getRadius() const { return radius; }
	void setRadius(float r) { radius = r; }
private:
	float radius;
};
