#ifndef __INPUT_SYSTEM__
#define __INPUT_SYSTEM__

#include "systems/system.h"

namespace NN {
  class Engine;
  namespace Systems::BuiltIns {
    class InputSystem: public System {
      public:
        void update(NN::Engine &engine, double frameTime) override;
    };
  }
}
#endif
