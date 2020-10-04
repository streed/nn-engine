#include "systems/system.h"


namespace NN {
  class Engine;
  namespace Systems {
    namespace BuiltIns {
      class PhysicsSystem: public NN::Systems::System {
        public:
          void update(Engine &engine, double frameTime) override;
      };
    }
  }
}
