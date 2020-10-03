#include "systems/system.h"


namespace NN {
  class Coordinator;
  namespace Systems {
    namespace BuiltIns {
      class PhysicsSystem: public NN::Systems::System {
        public:
          void update(Coordinator &coordinator, double frameTime) override;
      };
    }
  }
}
