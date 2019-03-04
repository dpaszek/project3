#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include "sprite.h"
#include "multisprite.h"
#include "twowaysprite.h"
#include "gameData.h"
#include "engine.h"
#include "frameGenerator.h"

Engine::~Engine() { 
  for(auto& t : spr)
  {
  	delete t;
  }
  std::cout << "Terminating program" << std::endl;
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IoMod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc.getRenderer() ),
  ocean("back", Gamedata::getInstance().getXmlInt("back/factor") ),
  mount("mountains", Gamedata::getInstance().getXmlInt("mountains/factor")),
  viewport( Viewport::getInstance() ),
  currentSprite(0),
  spr(8),
  makeVideo( false )
{
  spr[0] = new TwoWaySprite("shark");
  spr[0]->setScale(1.5);
  spr[1] = new TwoWaySprite("shark");
  spr[1]->setScale(1.5);
  spr[2] = new Sprite("starfish1");
  spr[2]->setScale(.3);
  spr[3] = new Sprite("starfish2");
  spr[3]->setScale(.4);
  spr[4] = new Sprite("starfish3");
  spr[4]->setScale(.4);
  spr[5] = new Sprite("fish1");
  spr[5]->setScale(.5);
  spr[6] = new Sprite("fish2");
  spr[6]->setScale(.5);
  spr[7] = new Sprite("fish3");
  spr[7]->setScale(.5);
   
  Viewport::getInstance().setObjectToTrack(spr[0]);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  ocean.draw();
  mount.draw();

  for(auto& t : spr)
  {
  	t->draw();
  }
  
  std::string fps;
  std::stringstream strm;
  
  strm << "fps: " << clock.getFps();
  fps = strm.str();
  io.writeText(fps, 20, 100);
  
  viewport.draw();
  SDL_RenderPresent(renderer);
}

void Engine::update(Uint32 ticks) {
  ocean.update();
  mount.update();
  
  for(auto& t : spr)
  {
  	t->update(ticks);
  }
  
  viewport.update(); // always update viewport last
}

void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % spr.size();
  Viewport::getInstance().setObjectToTrack(spr[currentSprite]);
}

void Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDL_SCANCODE_ESCAPE] || keystate[SDL_SCANCODE_Q]) {
          done = true;
          break;
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if ( keystate[SDL_SCANCODE_T] ) {
          switchSprite();
        }
        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
      }
    }

    // In this section of the event loop we allow key bounce:

    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
