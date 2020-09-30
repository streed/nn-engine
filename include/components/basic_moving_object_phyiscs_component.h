#ifndef __BASIC_MOVING_OBJECT_PHYSICS_COMPONENT__
#define __BASIC_MOVING_OBJECT_PHYSICS_COMPONENT__

#include "components/physics_component.h"

class GameObject;
class World;

class BasicMovingObjectPhysicsComponent: public PhysicsComponent {
  public:
    virtual void update(GameObject *object, Engine &engine, World &world, double frameTime);
};

#endif
