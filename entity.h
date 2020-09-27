#ifndef ENTITY_H
#define ENTITY_H

#include <vector>


class World;
class Player;

class Entity {
  public:
    double posX;
    double posY;
    Entity(double posX, double posY): posX(posX), posY(posY) {};
    
    virtual void update(World &world, Player *player, std::vector<Entity *> *entities, double timeDiff) {};
};

#endif
