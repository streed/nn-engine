#ifndef __PHYSICS_COMPONENT__
#define __PHYSICS_COMPONENT__

class BasicMovingObject;
class World;

class PhysicsComponent {
  public:
    virtual void update(BasicMovingObject &object, World &world, double frameTime) = 0;
};

#endif
