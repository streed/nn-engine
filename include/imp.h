#ifndef __IMP__
#define __IMP__

#include "entity_factory_registration.h"
#include "monster.h"

class Imp: public Monster {
  static Entities::Registration<Imp> registered;

  public:
    Imp(): Monster(100, 0, 0, 0) {};
};

#endif
