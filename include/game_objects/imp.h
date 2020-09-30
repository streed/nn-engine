#ifndef __IMP__
#define __IMP__

#include "game_objects/basic_moving_object.h"
#include "components/imp_ai_component.h"

class Imp: public BasicMovingObject {
  public:
    void update(Engine &engine, World &world, double frameTime);

  private:
    ImpAIComponent impAIComponent;
};

#endif
