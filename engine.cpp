#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include "sprite.h"
#include "multisprite.h"
#include "twoWayMultiSprite.h"
#include "sludge.h"
#include "gamedata.h"
#include "engine.h"
#include "player.h"
#include "frameGenerator.h"

Engine::~Engine() { 
  std::cout << "Terminating program" << std::endl;
  // for (auto& it : sprites) {
  //   delete it;
  // }
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IOmod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc->getRenderer() ),
  sound(),
  far("farbuildings", Gamedata::getInstance().getXmlInt("farbuildings/factor") ),
  back("backbuildings", Gamedata::getInstance().getXmlInt("backbuildings/factor") ),
  fore("foreground", Gamedata::getInstance().getXmlInt("foreground/factor") ),
  viewport( Viewport::getInstance() ),
  hud(0, Gamedata::getInstance().getXmlInt("HUD/x"),
    Gamedata::getInstance().getXmlInt("HUD/y"),
    Gamedata::getInstance().getXmlInt("HUD/width"),
    Gamedata::getInstance().getXmlInt("HUD/height")),
  poolhud(1, Gamedata::getInstance().getXmlInt("poolHUD/x"),
    Gamedata::getInstance().getXmlInt("poolHUD/y"),
    Gamedata::getInstance().getXmlInt("poolHUD/width"),
    Gamedata::getInstance().getXmlInt("poolHUD/height")),
  // sprites(),
  currentSprite(-1),

  makeVideo( Gamedata::getInstance().getXmlBool("makeVideo") ), gameover(false)
{
  ObjectManager::getInstance().initObjects();
  //sprites.push_back( new Sludge() );
  //switchSprite();
  Viewport::getInstance().setObjectToTrack(ObjectManager::getInstance().getObject("player"));
  std::cout << "Loading complete" << std::endl;
  hud.display(3000);
  poolhud.display(3000);
}

void Engine::draw() const {
  far.draw();
  back.draw();
  fore.draw();

  //for(auto* s : sprites) s->draw();
  ObjectManager::getInstance().drawObjects();

  viewport.draw();

  hud.draw(clock.getAvgFps());
  poolhud.draw(clock.getAvgFps());
  SDL_RenderPresent(renderer);
}

void Engine::update(Uint32 ticks) {
  ObjectManager::getInstance().updateObjects(ticks);
  //for(auto* s : sprites) s->update(ticks);
  far.update();
  back.update();
  fore.update();
  viewport.update(); // always update viewport last
  hud.update(ticks);
  poolhud.update(ticks);

  if (viewport.getFade() >= 10.0) {
    gameover = true;
  }
}

void Engine::switchSprite(){
  currentSprite++;
  currentSprite = currentSprite % ObjectManager::getInstance().getInstanceCount();
  Viewport::getInstance().setObjectToTrack(ObjectManager::getInstance().getObject(currentSprite));
}

void Engine::play() {
  SDL_Event event;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    while ( SDL_PollEvent(&event) ) {
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if (event.type == SDL_KEYDOWN) {
        switch ( event.key.keysym.sym ) {
          Player* p;
          case SDLK_ESCAPE:
            done = true;
            break;
          case SDLK_q:
            done = true;
            break;
          case SDLK_d:
            Gamedata::getInstance().updateRight(true);
            break;
          case SDLK_a:
            Gamedata::getInstance().updateLeft(true);
            break;
          case SDLK_w:
            p = (Player*) ObjectManager::getInstance().getObject("player");
            if (p) p->jump();
            break;
          case SDLK_SPACE:
            p = (Player*) ObjectManager::getInstance().getObject("player");
            if (p) {
              if (p->makeShield()) {
                sound[1];
              }
            }
            break;
          case SDLK_DOWN:
            p = (Player*) ObjectManager::getInstance().getObject("player");
            if (p) p->stop();
            break;
          case SDLK_p:
            if ( clock.isPaused() ) clock.unpause();
            else clock.pause();
            break;
          case SDLK_g:
            p = (Player*) ObjectManager::getInstance().getObject("player");
            if (p && !p->getGodMode())
              p->setGodMode(true);
            else if (p && p->getGodMode()) 
              p->setGodMode(false);
          // case SDLK_s:
          //   clock.toggleSloMo();
          //   break;
          case SDLK_RETURN:
            if (gameover) resetGame();
            break;
          case SDLK_r:
            resetGame();
            break;
          case SDLK_t:
            switchSprite();
            break;
          case SDLK_F1:
            if (hud.getTime() == 0) hud.display(1000000);
            else hud.display(0);
            break;
          case SDLK_F2:
            if (poolhud.getTime() == 0) poolhud.display(10000);
            else poolhud.display(0);
            break;
          case SDLK_F4:
            if (!makeVideo) {
              std::cout << "Initiating frame capture " << std::endl;
              makeVideo = true;
            } else if (makeVideo) {
              std::cout << "Terminating frame capture " << std::endl;
              //makeVideo = false;
            }
            break;
          default:
            break;
        }
        
      }
      if (event.type == SDL_KEYUP) {        
        switch (event.key.keysym.sym) {
          case SDLK_d:
            Gamedata::getInstance().updateRight(false);
            break;
          case SDLK_a:
            Gamedata::getInstance().updateLeft(false);
            break;
          default:
            break;
        }
      }
      if (event.type == SDL_MOUSEMOTION) {
        Gamedata::getInstance().updateMouse(event.motion.x, event.motion.y);
      }
    }

    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      if (!gameover) {
        clock.incrFrame();
        update(ticks);
        draw();
        if ( makeVideo ) {
          frameGen.makeFrame();
        }
      }
      else if (!clock.isPaused()) {
        clock.pause();
      }
    }
  }
}

void Engine::resetGame() {
  ObjectManager::reset();
  ObjectManager::getInstance().initObjects();
  Viewport::getInstance().setObjectToTrack(ObjectManager::getInstance().getObject("player"));
  sound.stopMusic();
  sound.startMusic();
  std::cout << "Loading complete" << std::endl;
  hud.display(3000);
  poolhud.display(3000);
  viewport.setFade(0);
}