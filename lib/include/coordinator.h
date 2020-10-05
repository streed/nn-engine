#ifndef __COORDINATOR__
#define __COORDINATOR__

#include <memory>

#include "entities.h"
#include "components.h"
#include "systems/system.h"

namespace NN {
  class Coordinator {
    public:
      void init();
      Entities::Entity createEntity();
      void destroyEntity(Entities::Entity entity);

      template<typename T> void registerComponent() {
        componentManager->registerComponent<T>();
      }

      template<typename T> void addComponent(Entities::Entity entity, T component) {
        componentManager->addComponent<T>(entity, component);
        auto signature = entityManager->getSignature(entity);
        signature.set(componentManager->getComponentType<T>(), true);
        entityManager->setSignature(entity, signature);
        systemManager->entitySignatureChanged(entity, signature);
      }

      template<typename T> void removeComponent(Entities::Entity entity) {
        componentManager->removeComponent<T>(entity);
        auto signature = entityManager->getSignature(entity);
        signature.set(componentManager->getComponentType<T>(), false);
        entityManager->setSignature(entity, signature);
        systemManager->entitySignatureChanged(entity, signature);
      }

      template<typename T>T& getComponent(Entities::Entity entity) {
        return componentManager->getComponent<T>(entity);
      }

      template<typename T>Components::ComponentType getComponentType() {
        return componentManager->getComponentType<T>();
      }

      template<typename T> std::shared_ptr<T> registerSystem() {
        return systemManager->registerSystem<T>();
      }

      template<typename T> void setSystemSignature(Entities::Signature signature) {
        systemManager->setSignature<T>(signature);
      }

    private:
      std::unique_ptr<Components::ComponentManager> componentManager;
      std::unique_ptr<Entities::EntityManager> entityManager;
      std::unique_ptr<Systems::SystemManager> systemManager;
  };
}

#endif
