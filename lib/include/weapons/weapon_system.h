#ifndef NN_WEAPON_SYSTEM_H
#define NN_WEAPON_SYSTEM_H

#include <vector>
#include "systems/system.h"
#include "weapons/weapon_components.h"

namespace NN {
  class Engine;

  namespace Systems::Weapons {

    class WeaponSystem : public Systems::System {
      public:
        void update(NN::Engine *engine, double frameTime) override;

        // Get recent damage events (cleared each frame)
        const std::vector<Components::DamageEvent> &getDamageEvents() const {
          return damageEvents;
        }

        // Entities that can be hit by hitscan (must have Position + AnimatedSprite).
        // Populated by the scene or engine during setup.
        std::set<Entities::Entity> hittableEntities;

      private:
        void handleHitscan(Engine *engine, Entities::Entity entity, double frameTime);
        void handleProjectileFire(Engine *engine, Entities::Entity entity, double frameTime);

        std::vector<Components::DamageEvent> damageEvents;
    };

  }
}

#endif
