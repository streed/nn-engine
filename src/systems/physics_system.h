#include "systems/physics_system.h"

#include "engine/engine.h"
#include "coordinator.h"
#include "components.h"

namespace NN {
  namespace Systems {
    namespace BuiltIns {
      void PhysicsSystem::update(Engine &engine, double frameTime) {
        Coordinator *coordinator = engine.getCoordinator();
        for (auto const &entity: entities) {
          auto &position = coordinator->getComponent<NN::Components::Position>(entity);

          position.posX += 1;
          position.posY += 1;
        }
      }
    }
  }
}
