#include <iostream>
#include <cmath>
#include "explodingSprite.h"
#include "spriteSheet.h"

ExplodingSprite::ExplodingSprite(const Sprite& s) :
  Sprite(s), 
  chunks(), 
  freeList(){
  makeChunks(
    Gamedata::getInstance().getXmlInt(s.getName()+"/chunk/size")
  );
}

ExplodingSprite::~ExplodingSprite() { 
  for ( auto c : chunks ) delete c;
  for ( auto f : freeList ) delete f;
  chunks.clear();     // clearing a vector is a waste of time, but ...
  freeList.clear();   // still ...
}

void ExplodingSprite::draw() const { 
  // Override draw; use the draw in Chunk, which derives from Sprite.
  // So the draw we're using is actually in Sprite
  for ( Chunk* c : chunks ) {
    c->draw();
  }
}

void ExplodingSprite::update(Uint32 ticks) { 
  std::list<Chunk*>::iterator ptr = chunks.begin();
  while (ptr != chunks.end()) {
    (*ptr)->update(ticks);
    if ((*ptr)->goneTooFar()) {  // Check to see if we should free a chunk
      freeList.push_back(*ptr);
      ptr = chunks.erase(ptr);
    }   
    else ++ptr;
  }
}

void ExplodingSprite::makeChunks(unsigned int n) { 
  // Break the SDL_Surface into n*n squares; where each square
  // has width and height of frameWidth/n and frameHeight/n
  int chunk_width = std::max(1u, getFrame()->getWidth()/n);
  int chunk_height = std::max(1u, getFrame()->getHeight()/n);
  int speedx = static_cast<int>(getVelocityX()); // Wanna test for zero...
  int speedy = static_cast<int>(getVelocityY()); // Make sure it's an int.
  if (speedx == 0) speedx = 1; // Make sure it's not 0;
  if (speedy == 0) speedy = 1; // Make sure it's not 0;

  const Frame* proto = getFrame();

  // Read the SDL_Surface so we can chunk it:
  int source_y = 0;
  while  (source_y+chunk_height < getFrame()->getHeight() ) {
    int source_x = 0;
    while ( source_x+chunk_width < getFrame()->getWidth() ) {
      // Give each chunk it's own speed/direction:
      float sx = (rand() % speedx + 40) * (rand()%2?-1:1); // 'cause %0 is 
      float sy = (rand() % speedy + 40) * (rand()%2?-1:1); // float except

      Frame* frame = proto->crop({source_x,source_y,chunk_width,chunk_height});
      Chunk* chunk = new Chunk(
                Vector2f(getX()+source_x,   // x coord of destination 
                         getY()+source_y),  // y coord of destination
                Vector2f(sx, sy),
                getName(),
                frame);
      chunks.push_back(chunk);
      source_x += chunk_width;
    }
    source_y += chunk_height;
  }
}

