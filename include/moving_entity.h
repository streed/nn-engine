#ifndef MOVING_ENTITY_H
#define MOVING_ENTITY_H

#include "entity.h"

class MovingEntity: public Entity {
  public:
    double accelerationConstant;
    double currentAcceleration;
    double currentSpeed;
    double maxSpeedClip; 
    double maxRotateSpeedClip;

    MovingEntity(): Entity(),
                    accelerationConstant(0),
                    maxSpeedClip(0),
                    maxRotateSpeedClip(0) {};

    MovingEntity(double posX,
                 double posY,
                 double accelerationConstant,
                 double maxSpeedClip,
                 double maxRotateSpeedClip): Entity(posX, posY), 
                                             accelerationConstant(accelerationConstant),
                                             maxSpeedClip(maxSpeedClip),
                                             maxRotateSpeedClip(maxRotateSpeedClip) {};
};

#endif