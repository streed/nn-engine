#ifndef __PLAYER_MOVEMENT_SYSTEM__
#define __PLAYER_MOVEMENT_SYSTEM__

#include "systems/system.h"

namespace NN {
  class Engine;
  namespace Systems::BuiltIns {
    class PlayerMovementSystem: public System {
      public:
        void update(NN::Engine *engine, double frameTime) override;
    };
  }
}

#endif
