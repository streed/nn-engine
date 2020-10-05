#include "systems/system.h"

namespace NN {
  namespace Systems {
    void SystemManager::destroyedEntity(Entities::Entity entity) {
      for (auto const &pair: systems) {
        auto const &system = pair.second;
        system->entities.erase(entity);
      }
    }

    void SystemManager::entitySignatureChanged(Entities::Entity entity, Entities::Signature signature) {
      for (auto const &pair: systems) {
        auto const &type = pair.first;
        auto const &system = pair.second;
        auto const &systemSignature = signatures[type];

        if ((signature & systemSignature) == systemSignature) {
          system->entities.insert(entity);
        } else {
          system->entities.erase(entity);
        }
      }
    }
  }
};
