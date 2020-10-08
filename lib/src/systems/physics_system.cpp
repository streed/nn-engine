#include <iostream>
using namespace std;

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

      if (engine->getWorld()->isTraversable(int(newX), int(position.posY))) {
        position.posX = newX;
      }

      if (engine->getWorld()->isTraversable(int(position.posX), int(newY))) {
        position.posY = newY;
      }
    }
  }
}
