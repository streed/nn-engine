#ifndef ENTITY_H
#define ENTITY_H

#include <vector>


class World;
class Player;
class Game;

class Entity {
  public:
    double posX;
    double posY;
    Entity(): posX(0), posY(0) {};
    Entity(double posX, double posY): posX(posX), posY(posY) {};
    
    virtual void update(Game *game, World &world, Player *player, std::vector<Entity *> *entities, double timeDiff) {};
};

#endif
