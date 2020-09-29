#ifndef __SPRITE__
#define __SPRITE__

#include "moving_entity.h"
#include "player.h"

class Sprite: public MovingEntity {
  private:
    int textureIndex;

  public:
    Sprite(double posX, double posY, int textureIndex): MovingEntity(posX, posY, 0, 0, 0),
                                                        textureIndex(textureIndex) {};
    Sprite(double posX,
           double posY,
           double accelerationConstant,
           double maxSpeedClip,
           double maxRotateSpeed,
           int textureIndex): MovingEntity(posX,
                                           posY,
                                           accelerationConstant,
                                           maxSpeedClip,
                                           maxRotateSpeed),
                              textureIndex(textureIndex) {};

    double distanceFromSquared(Player &player);
    int getTextureIndex();
};

#endif
