#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

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
