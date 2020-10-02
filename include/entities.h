#ifndef __ENTITIES__
#define __ENTITIES__

#include <array>
#include <queue>
#include <cstdlib>

namespace NN {
  namespace Entities {
    using Entity = std::uint32_t;
    const Entity MAX_ENTITIES = 1024;

    class EntityManager {
      public:
        EntityManager() {
          for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
          }
        }

      private:
        std::queue<Entity> availableEntities();
        std::array<Signature, MAX_ENTITIES> signatures();
        uint32_t livingEntityCount{};
    };
  };
};

#endif
