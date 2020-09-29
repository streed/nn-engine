#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "input_packet.h"
#include "world.h"
#include "moving_entity.h"

class Player: public MovingEntity {
  public:
    Camera *camera;
    double diameter = 1.0;// This is used for projectile collision.

    Player(Camera *camera,
           double posX,
           double posY,
           double accelerationConstant,
           double maxSpeedClip,
           double maxRotateSpeedClip);
    int getMapX();
    int getMapY();
    void handleInputs(InputPacket inputPacket, World world, double frameTime);
    double getDiameter();
};

#endif
