#ifndef __BASIC_MOVING_OBJECT_PHYSICS_COMPONENT__
#define __BASIC_MOVING_OBJECT_PHYSICS_COMPONENT__

#include "managers/components_registration.h"
#include "components/physics_component.h"

class GameObject;
class World;

class BasicMovingObjectPhysicsComponent: public PhysicsComponent {
  static Components::Registration<BasicMovingObjectPhysicsComponent> registered;

  public:
    virtual void update(GameObject *object, Engine &engine, World &world, double frameTime);
};

#endif
