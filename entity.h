#ifndef ENTITY_H
#define ENTITY_H

#include <vector>

#include "world.h"

class Entity {
  public:
    double posX;
    double posY;
    Entity(double posX, double posY): posX(posX), posY(posY) {};
    
    void update(World &world, std::vector<Entity *> entities) {};
};

#endif
