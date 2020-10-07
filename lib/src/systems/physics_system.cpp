#include "systems/physics_system.h"

#include "engine/engine.h"
#include "coordinator.h"
#include "components.h"
#include "world.h"

namespace NN::Systems::BuiltIns {
  void PhysicsSystem::update(Engine *engine, double frameTime) {
    Coordinator *coordinator = engine->getCoordinator();
    for (auto const &entity: entities) {
      auto &position = coordinator->getComponent<NN::Components::Position>(entity);
      auto &velocity = coordinator->getComponent<NN::Components::Velocity>(entity);

      double newX = position.posX + velocity.velocityX;
      double newY = position.posY + velocity.velocityY;

      // TODO: Make sure this works with isTraversable
      if (engine->getWorld()->getMapPoint(int(newX), int(position.posY)) == 0) {
        position.posX = newX;
      }

      if (engine->getWorld()->getMapPoint(int(position.posX), int(newY)) == 0) {
        position.posY = newY;
      }
    }
  }
}
