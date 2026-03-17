#ifndef NN_ENTITIES_H
#define NN_ENTITIES_H

#include <array>
#include <cstdint>
#include <queue>
#include <cstdlib>
#include <bitset>

namespace NN {
  namespace Entities {
    using Entity = std::uint32_t;
    const Entity MAX_ENTITIES = 1024;
    using Signature = std::bitset<32>;

    class EntityManager {
      public:
        EntityManager() {
          for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
            availableEntities.push(entity);
          }
        }

        Entity create() {
          Entity id = availableEntities.front();
          availableEntities.pop();
          livingEntityCount++;
          return id;
        }

        void destroyEntity(Entity entity) {
          signatures[entity].reset();
          availableEntities.push(entity);
          --livingEntityCount;
        }

        void setSignature(Entity entity, Signature signature) {
          signatures[entity] = signature;
        }

        Signature getSignature(Entity entity) {
          return signatures[entity];
        }

      private:
        std::queue<Entity> availableEntities{};
        std::array<Signature, MAX_ENTITIES> signatures{};
        uint32_t livingEntityCount{};
    };
  };
};

#endif
