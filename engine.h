#include <vector>
#include <SDL.h>
#include "ioMod.h"
#include "renderContext.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "objectManager.h"
#include "HUD.h"
#include "sound.h"

class Engine {
public:
  Engine ();
  ~Engine ();
  void play();
  void switchSprite();

  bool getGameOver() const { return gameover; }
private:
  const RenderContext* rc;
  const IOmod& io;
  Clock& clock;

  SDL_Renderer * const renderer;
  SDLSound sound;  
  World far, back, fore;
  Viewport& viewport;
  HUD hud, poolhud;

  // ObjectManager objectManager;
  // std::vector<Drawable*> sprites;

  int currentSprite;
  bool makeVideo, gameover;

  void draw() const;
  void update(Uint32);
  void resetGame();

  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
};
