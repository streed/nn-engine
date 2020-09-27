#ifndef __MONSTER__
#define __MONSTER__

#include <map>
#include <vector>

#include "sprite.h"

typedef std::pair<int, int> Point;

class Monster: public Sprite {
  bool seeking;
  double searchDistance = 4;//Sqaured to not need the sqrt call

  public:
    Monster(double posX, double posY, int textureIndex): Sprite(posX, posY, 0, 2, 0, textureIndex) {}

    virtual void update(World &world, Player *player, std::vector<Entity *> *entities, double timeDiff) ;

  private:
    Point findNextCellToMoveTo(World &world, Player *player);
    std::vector<Point> getNeighbors(World &world, Point, std::map<Point, Point> Q);
};

#endif
