#include "coordinator.h"

namespace NN {
  void Coordinator::init() {
    componentManager = std::make_unique<Components::ComponentManager>();
    entityManager = std::make_unique<Entities::EntityManager>();
    systemManager = std::make_unique<Systems::SystemManager>();
  }

  Entities::Entity Coordinator::createEntity() {
    return entityManager->create();
  }

  void Coordinator::destroyEntity(Entities::Entity entity) {
    entityManager->destroyEntity(entity);
    componentManager->destroyedEntity(entity);
    systemManager->destroyedEntity(entity);
  }

};
