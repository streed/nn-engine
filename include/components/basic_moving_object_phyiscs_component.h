#ifndef __BASIC_MOVING_OBJECT_PHYSICS_COMPONENT__
#define __BASIC_MOVING_OBJECT_PHYSICS_COMPONENT__

#include "components/physics_component.h"

class BasicMovingObject;
class World;

class BasicMovingObjectPhysicsComponent: public PhysicsComponent {
  public:
    virtual void update(BasicMovingObject &object, World &world, double frameTime);
};

#endif
