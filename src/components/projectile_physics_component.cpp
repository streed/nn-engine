#include <iostream>
using namespace std;

#include "components/projectile_physics_component.h"
#include "game_objects/game_object.h"
#include "game_objects/projectile_object.h"
#include "engine.h"
#include "world.h"

Components::Registration<ProjectilePhysicsComponent> ProjectilePhysicsComponent::registered = Components::Registration<ProjectilePhysicsComponent>("ProjectilePhysicsComponent");

void ProjectilePhysicsComponent::update(GameObject *object, Engine &engine, World &world, double frameTime) {
  ProjectileObject *projectile = dynamic_cast<ProjectileObject *>(object);

  projectile->lifeSpan -= frameTime;

  int mapX = int(projectile->posX);
  int mapY = int(projectile->posY);

  if (projectile->lifeSpan > 0 && world.getMapPoint(mapX, mapY) == 0) {
    double speed = projectile->maxSpeed * frameTime;
    projectile->posX = projectile->posX + projectile->dirX * speed;
    projectile->posY = projectile->posY + projectile->dirY * speed;
  } else {
    engine.removeGameObject(object);
  }
}
