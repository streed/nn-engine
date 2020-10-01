#ifndef __PHYSICS_COMPONENT__
#define __PHYSICS_COMPONENT__

#include "components/component.h"

class Engine;
class GameObject;
class World;

class PhysicsComponent: public Component {
  public:
    virtual void update(GameObject *object, Engine &engine, World &world, double frameTime) = 0;
};

#endif
