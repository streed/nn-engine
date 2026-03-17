#include <cmath>

#include "weapons/projectile_system.h"
#include "engine/engine.h"
#include "coordinator.h"
#include "components.h"
#include "world.h"

namespace NN::Systems::Weapons {

  void ProjectileSystem::update(NN::Engine *engine, double frameTime) {
    damageEvents.clear();
    entitiesToDestroy.clear();

    Coordinator *coordinator = engine->getCoordinator();
    World *world = engine->getWorld();

    for (auto const &entity : entities) {
      auto &projectile = coordinator->getComponent<Components::Projectile>(entity);
      auto &position = coordinator->getComponent<Components::Position>(entity);

      // Update age
      projectile.age += frameTime;

      // Check lifetime expiry
      if (projectile.age >= projectile.lifetime) {
        entitiesToDestroy.push_back(entity);
        continue;
      }

      // Move projectile
      double newX = position.posX + projectile.dirX * projectile.speed * frameTime;
      double newY = position.posY + projectile.dirY * projectile.speed * frameTime;

      // Check wall collision
      if (!world->isTraversable(static_cast<int>(newX), static_cast<int>(newY))) {
        entitiesToDestroy.push_back(entity);
        continue;
      }

      position.posX = newX;
      position.posY = newY;

      // Check entity collision (circle-circle intersection)
      // We check against all entities that have Position and Health
      // Since we can't iterate Health entities from here, we expose damage events
      // and let the scene or a dedicated collision system handle resolution
    }

    // Destroy expired/collided projectiles
    for (auto const &entity : entitiesToDestroy) {
      coordinator->destroyEntity(entity);
    }
  }

}
