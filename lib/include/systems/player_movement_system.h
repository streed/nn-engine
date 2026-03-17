#ifndef NN_PLAYER_MOVEMENT_SYSTEM_H
#define NN_PLAYER_MOVEMENT_SYSTEM_H

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
