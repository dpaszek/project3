#include "twowaysprite.h"
#include "gameData.h"
#include "imageFactory.h"
#include <string>

void TwoWaySprite::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

TwoWaySprite::TwoWaySprite( const std::string& name) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x"), 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY"))
           ),
  imagesRight( ImageFactory::getInstance().getImages(name) ),
  imagesLeft( ImageFactory::getInstance().getImages(name+"Left") ),
  images(imagesRight),
  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/frameInterval")),
  timeSinceLastFrame( 0 ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

TwoWaySprite::TwoWaySprite( const TwoWaySprite& s) :
  Drawable(s.getName(), s.getPosition(), s.getVelocity()),
  imagesRight(s.imagesRight),
  imagesLeft(s.imagesLeft),
  images(s.imagesRight),
  currentFrame(s.currentFrame),
  numberOfFrames(s.numberOfFrames),
  frameInterval(s.frameInterval),
  timeSinceLastFrame(s.timeSinceLastFrame),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

void TwoWaySprite::draw() const { 
  images[currentFrame]->draw(getX(), getY(), getScale());
}

void TwoWaySprite::update(Uint32 ticks) { 
  advanceFrame(ticks);

  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if ( getY() < 0) {
    setVelocityY( fabs( getVelocityY() ) );
  }
  if ( getY() + getScaledHeight() > worldHeight) {
    setVelocityY( -fabs( getVelocityY() ) );
  }

  if ( getX() < 0) {
    setVelocityX( fabs( getVelocityX() ) );
    images = imagesRight;
  }
  if ( getX() + getScaledWidth() > worldWidth) {
    setVelocityX( -fabs( getVelocityX() ) );
    images = imagesLeft;
  }  

}
