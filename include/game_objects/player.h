#ifndef __PLAYER__
#define __PLAYER__

#include "components/input_component.h"
#include "components/physics_component.h"

#include "game_objects/basic_moving_object.h"

#include "moving_entity.h"

class Camera;
class Engine;
class World;


class Player: public BasicMovingObject {
  public:
    Player(Camera *camera,
           double posX,
           double posY,
           double maxSpeed,
           double maxRotateSpeed,
           InputComponent *inputComponent,
           PhysicsComponent *physicsComponent): BasicMovingObject(posX, posY, maxSpeed, maxRotateSpeed),
                                                camera(camera),
                                                inputComponent(inputComponent),
                                                physicsComponent(physicsComponent) {}

    void update(Engine &engine, World &world, double frameTime);

    Camera *camera;

  private:
    InputComponent *inputComponent;
    PhysicsComponent *physicsComponent;
};

#endif
