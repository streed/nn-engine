#include "systems/physics_system.h"

#include "coordinator.h"
#include "components.h"

namespace NN {
  namespace Systems {
    namespace BuiltIns {
      void PhysicsSystem::update(Coordinator &coordinator, double frameTime) {
        for (auto const &entity: entities) {
          auto &position = coordinator.getComponent<NN::Components::Position>(entity);

          position.posX += 1;
          position.posY += 1;
        }
      }
    }
  }
}
