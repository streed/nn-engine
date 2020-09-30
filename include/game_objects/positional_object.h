#ifndef __POSITIONAL_OBJECT__
#define __POSITIONAL_OBJECT__

#include "game_objects/game_object.h"

class PositionalObject: public GameObject {
  public:
    PositionalObject(): posX(0), posY(0) {}
    PositionalObject(double posX, double posY): posX(posX),
                                                posY(posY) {}

    double posX;
    double posY;
};

#endif
