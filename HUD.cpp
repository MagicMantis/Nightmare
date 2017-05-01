#include <sstream>
#include "HUD.h"
#include "ioMod.h"
#include "viewport.h"
#include "objectManager.h"
#include <string>

void HUD::update(Uint32 ticks) {
	time -= ticks;
	if (time < 0) time = 0;
}

void HUD::draw(int fps) const {
	if (time > 0) {
		SDL_Renderer* rend = RenderContext::getInstance()->getRenderer();
		SDL_Rect dst = {x,y,width,height};
		SDL_SetRenderDrawBlendMode(rend,SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(rend, 155,155,155,100);
		SDL_RenderFillRect(rend,&dst);
		SDL_SetRenderDrawColor(rend, 100,100,100,255);
		SDL_RenderDrawRect(rend,&dst);

		IOmod& io = IOmod::getInstance();
		//basic game info hud
		if (HUDid == 0) {
	  		io.writeText("Tracking: "+Viewport::getInstance().getObjectToTrack()->getName(), x+5, y+10);

			std::stringstream strm;
			strm << "Average FPS: " << fps;
			io.writeText(strm.str(), x+5, y+30);
			io.writeText("A: run left", x+5, y+50);
			io.writeText("D: run right", x+5, y+70);
			io.writeText("W: jump (double jump)", x+5, y+90);
			io.writeText("Space: blast", x+5, y+110);
			io.writeText("Avoid the sludge and",x+5, y+140);
			io.writeText("get to the end",x+5, y+160);
			io.writeText("of the level!", x+5, y+180);
		}

		//object pool hud
		if (HUDid == 1) {
			//auto active =;
			//int activeCount = 0;
			//if (active) activeCount = active->size();
			io.writeText("Sludge Pool", x+5, y+10);
			io.writeText("Active: "+std::to_string(ObjectManager::getInstance().getObjectsOfType("sludge")->size()), x+5, y+30);
			io.writeText("Free: "+std::to_string(ObjectManager::getInstance().getFreeList().size()), x+5, y+50);
		}
  		
	}
}

void HUD::display(int microseconds) {
	time = microseconds;
}