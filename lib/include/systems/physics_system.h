#ifndef NN_PHYSICS_SYSTEM_H
#define NN_PHYSICS_SYSTEM_H

#include "systems/system.h"

namespace NN {
  class Engine;
  namespace Systems::BuiltIns {
    class PhysicsSystem: public Systems::System {
      public:
        void update(Engine *engine, double frameTime) override;
    };
  }
}

#endif
