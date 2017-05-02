#include <sstream>
#include "viewport.h"
#include "ioMod.h"
#include "renderContext.h"

Viewport& Viewport::getInstance() {
  static Viewport viewport;
  return viewport;
}

Viewport::Viewport() : 
  gdata(Gamedata::getInstance()),
  position(0, 0),
  worldWidth(gdata.getXmlInt("world/width")),
  worldHeight(gdata.getXmlInt("world/height")),
  viewWidth(gdata.getXmlInt("view/width")), 
  viewHeight(gdata.getXmlInt("view/height")),
  objWidth(0), objHeight(0),
  rd(), mt(rd()),
  jitterScale(0.0),
  fade(0.0),
  objectToTrack(NULL)
{}

void Viewport::setObjectToTrack(const Drawable *obj) { 
  objectToTrack = obj; 
  if (!obj) return;
  objWidth = objectToTrack->getFrame()->getWidth();
  objHeight = objectToTrack->getFrame()->getHeight();
}

void Viewport::draw() const {
  IOmod& io = IOmod::getInstance();
  //io.writeText("Tracking "+objectToTrack->getName(), 30, 30);

  SDL_Renderer* rend = RenderContext::getInstance()->getRenderer();
  SDL_SetRenderDrawColor(rend,0,0,0,(int)(255*(fade/13)));
  SDL_Rect rect = {0,0,
    Gamedata::getInstance().getXmlInt("view/width"),
    Gamedata::getInstance().getXmlInt("view/height")};
  SDL_RenderFillRect(rend, &rect);
  if (fade == 10) {
    SDL_SetRenderDrawColor(rend,255,255,255,255);
    io.writeText("You Lose", viewWidth/2 - 50, viewHeight/2 - 10);
    io.writeText("Press Enter to Play Again...", viewWidth/2- 110, viewHeight/2 + 20);
  }

  int nameWidth = gdata.getXmlInt("nameWidth");
  int titleWidth = gdata.getXmlInt("titleWidth");
  SDL_Color red = {255,0,0,255};
  io.writeText("Joseph Savold", 10, 420, &red, nameWidth);
  io.writeText("Nightmare", 10, 440, &red, titleWidth);
}

void Viewport::update() {
  float x = 0;
  float y = 0;
  if (objectToTrack) {
    x = objectToTrack->getX();
    y = objectToTrack->getY();

    position[0] = (x + objWidth/2) - viewWidth/2;
    position[1] = (y + objHeight/2) - viewHeight/2;

    //jitter
    std::normal_distribution<float>dist(0.0,jitterScale);
    position[0] += dist(mt);
    position[1] += dist(mt);
    
  }

  if (position[0] < 0) position[0] = 0;
  if (position[1] < 0) position[1] = 0;
  if (position[0] > (worldWidth - viewWidth)) {
    position[0] = worldWidth-viewWidth;
  }
  if (position[1] > (worldHeight - viewHeight)) {
    position[1] = worldHeight-viewHeight;
  }

}