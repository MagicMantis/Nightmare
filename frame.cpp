#include "drawable.h"
#include "frame.h"
#include "ioMod.h"
#include "viewport.h"
#include "renderContext.h"

Frame::Frame( SDL_Texture* tex ) : 
  renderer(RenderContext::getInstance()->getRenderer()),
  texture( tex ),
  rect{0,0,0,0}
{ SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h); }

Frame::Frame( const Frame& frame ) :
  renderer(frame.renderer),
  texture(frame.texture), 
  rect(frame.rect)
{ }


Frame& Frame::operator=(const Frame& rhs) {
  renderer = rhs.renderer;
  texture = rhs.texture;
  rect = rhs.rect;
  return *this;
}

void Frame::draw(int x, int y) const {
  draw(x,y,false,1.0f);
}

void Frame::draw(int x, int y, bool doFlip) const {
  draw(x, y, doFlip, 1.0f);
}

void Frame::draw(int x, int y, bool doFlip, float scale) const {
  x -= Viewport::getInstance().getX();
  y -= Viewport::getInstance().getY();
  SDL_Rect dest = {x, y, (int)(rect.w*scale), (int)(rect.h*scale)};
  SDL_RendererFlip flip = (doFlip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
  SDL_RenderCopyEx(renderer, texture, NULL, &dest, 0, NULL, flip);
}

void Frame::draw(int sx, int sy, int dx, int dy) const {
  SDL_Rect src = { sx, sy, rect.w, rect.h };    
  SDL_Rect dst = { dx, dy, rect.w, rect.h };
  SDL_RenderCopy(renderer, texture, &src, &dst);
}

Frame* Frame::crop(SDL_Rect sub)const{
  if(sub.x+sub.w > rect.w || sub.y+sub.h > rect.h){
    std::cerr << "Attempted to crop image with invalid geometry."
              << std::endl
              << "(0,0 + "<<rect.w << "x"<<rect.h<<")"
              << " --> "
              << "("<<sub.x<<","<<sub.y<<" + "<<sub.w << "x"<<sub.h<<")"
              << std::endl;
    return nullptr;
  }

  Frame* cloned = new Frame(*this);
  cloned->rect = sub;

  return cloned;
}

