#ifndef NN_INPUT_SYSTEM_H
#define NN_INPUT_SYSTEM_H

#include "systems/system.h"

namespace NN {
  class Engine;
  namespace Systems::BuiltIns {
    class InputSystem: public System {
      public:
        void update(NN::Engine *engine, double frameTime) override;
    };
  }
}
#endif
