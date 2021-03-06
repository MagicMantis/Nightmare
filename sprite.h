#ifndef SPRITE__H
#define SPRITE__H
#include <string>
#include "collider.h"

class Sprite : public Collider {
public:
  Sprite(const std::string&);
  Sprite(const std::string&, const Vector2f&, float=1.0);
  Sprite(const Sprite&);
  virtual ~Sprite() { } 
  Sprite& operator=(const Sprite&);

  virtual const Frame* getFrame() const { return frame; }
  void setFrame(const Frame* f) { frame = f; }
  virtual void draw() const;
  virtual void update(Uint32 ticks);
  void explode(bool=true);
protected:
  int getWorldWidth() const { return worldWidth; }
  int getWorldHeight() const { return worldHeight; }
  int getFrameWidth() const { return frameWidth; }
  int getFrameHeight() const { return frameHeight; }
  float getScale() const { return scale; }
  void setScale(float s) { scale = s; }
private:
  const Frame * frame;
  int worldWidth;
  int worldHeight;
  int frameWidth;
  int frameHeight;
  float scale;
};
#endif
