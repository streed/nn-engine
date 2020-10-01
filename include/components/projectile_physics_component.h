#ifndef __PROJECTILE_PHYSICS_COMPONENT__
#define __PROJECTILE_PHYSICS_COMPONENT__

#include "components/physics_component.h"
#include "managers/components_registration.h"

class ProjectileObject;

class ProjectilePhysicsComponent: public PhysicsComponent {
  static Components::Registration<ProjectilePhysicsComponent> registered;

  public:
    void update(GameObject *object, Engine &engine, World &world, double frameTime);
};

#endif
