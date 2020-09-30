#ifndef __BASIC_MOVING_OBJECT__
#define __BASIC_MOVING_OBJECT__

#include "game_objects/positional_object.h"

class BasicMovingObject: public PositionalObject {
  public:
    BasicMovingObject(): PositionalObject(0, 0),
                         velocityX(0),
                         velocityY(0),
                         maxSpeedClip(0),
                         maxRotateSpeedClip(0) {}

    BasicMovingObject(double posX,
                      double posY,
                      double maxSpeedClip,
                      double maxRotateSpeedClip): PositionalObject(posX, posY),
                                                  velocityX(0),
                                                  velocityY(0),
                                                  maxSpeedClip(maxSpeedClip),
                                                  maxRotateSpeedClip(maxRotateSpeedClip) {}

    BasicMovingObject(const BasicMovingObject &b) {
       posX = b.posX;
       posY = b.posY;
       velocityX = b.velocityX;
       velocityX = b.velocityY;
       maxSpeedClip = b.maxSpeedClip;
       maxRotateSpeedClip = b.maxRotateSpeedClip;
    }

    double velocityX;
    double velocityY;
    double maxSpeedClip;
    double maxRotateSpeedClip;
};

#endif
