#ifndef __MONSTER__
#define __MONSTER__

#include <map>
#include <vector>

#include "sprite.h"

class Engine;

typedef std::pair<int, int> Point;

class Monster: public Sprite {
  bool seeking;
  double searchDistance = 3.5;//Sqaured to not need the sqrt call

  static constexpr double shootingCoolDownConstant = 2;// Allowed to fire every 2secs;
  double timeUntilNextShot = 0;

  int health;

  public:
    Monster(int health, double posX, double posY, int textureIndex): Sprite(posX, posY, 0, 2, 0, textureIndex),
                                                                     health(health) {}

    virtual void update(Engine *engine, World &world, Player *player, std::vector<Entity *> *entities, double timeDiff);

  private:
    Point findNextCellToMoveTo(World &world, Player *player);
    std::vector<Point> getNeighbors(World &world, Point);
};

#endif
