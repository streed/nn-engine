#ifndef NN_PROJECTILE_SYSTEM_H
#define NN_PROJECTILE_SYSTEM_H

#include <vector>
#include "systems/system.h"
#include "weapons/weapon_components.h"

namespace NN {
  class Engine;

  namespace Systems::Weapons {

    class ProjectileSystem : public Systems::System {
      public:
        void update(NN::Engine *engine, double frameTime) override;

        const std::vector<Components::DamageEvent> &getDamageEvents() const {
          return damageEvents;
        }

      private:
        std::vector<Components::DamageEvent> damageEvents;
        std::vector<Entities::Entity> entitiesToDestroy;
    };

  }
}

#endif
