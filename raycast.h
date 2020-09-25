#ifndef RAYCAST_H
#define RAYCAST_H

#include "world.h"

struct RayCastHit {
  int mapX;
  int mapY;
  double wallX;
  int texX;
  int side;
  double perpWallDist;
};

class RayCast {
  double startX;
  double startY;
  double dirX;
  double dirY;

  public:
    RayCast(double startX, double startY, double dirX, double dirY): startX(startX),
                                                                     startY(startY),
                                                                     dirX(dirX),
                                                                     dirY(dirY) {};

    RayCastHit collideWorld(World &world);
};

#endif
