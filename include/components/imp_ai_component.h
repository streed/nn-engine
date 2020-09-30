#ifndef __IMP_AI_COMPONENT__
#define __IMP_AI_COMPONENT__

#include "components/ai_component.h"

class Player;
class World;
class Imp;

class ImpAIComponent: public AIComponent {
  public:
    void update(GameObject &object, Player &player, World &world, double frameTime);

    bool seeking;
    double searchDistance = 2.0;
    double timeUntilNextShot;
    static constexpr double shootingCoolDownConstant = 2.5;
};

#endif
