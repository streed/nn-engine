#ifndef PLAYER_H
#define PLAYER_H

#include "components/input_component.h"
#include "components/physics_component.h"

#include "moving_entity.h"

class Camera;
class World;


class Player {
  public:
    Camera *camera;

    Player(Camera *camera,
           double posX,
           double posY): camera(camera),
                         posX(posX),
                         posY(posY),
                         velocityX(0),
                         velocityY(0),
                         maxSpeedClip(5),
                         maxRotateSpeedClip(3) {}

    void handleInputs(World world, double frameTime);

    double posX;
    double posY;
    double velocityX;
    double velocityY;
    double maxSpeedClip;
    double maxRotateSpeedClip;

  private:
    InputComponent inputComponent;
    PhysicsComponent physicsComponent;
};

#endif
