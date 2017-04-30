#ifndef CHUNK__H
#define CHUNK__H

#include <iostream>
#include "sprite.h"
#include "gamedata.h"

class Chunk : public Sprite {
public:
  explicit Chunk( const Vector2f& pos, const Vector2f vel, 
                  const string& name, Frame* fm) :
    Sprite(name), 
    distance(0), 
    maxDistance(Gamedata::getInstance().getXmlInt(name+"/chunk/distance")), 
    tooFar(false),
    frame(fm)
  { 
    setFrame( fm );
    setPosition( pos );
    setVelocity( vel );
  }

  Chunk(const Chunk& )=default;
  Chunk(      Chunk&&)=default;
  Chunk& operator=(const Chunk& )=default;
  Chunk& operator=(      Chunk&&)=default;

  virtual ~Chunk(){ delete frame; }
  virtual void update(Uint32 ticks);
  bool goneTooFar() const { return tooFar; }
  void reset() {
    tooFar = false;
    distance = 0;
  }
private:
  float distance;
  float maxDistance;
  bool tooFar;
  Frame* frame;
};
#endif
