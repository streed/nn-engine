#ifndef RAYCAST_H
#define RAYCAST_H

#include <vector>
#include "world.h"
#include "entities.h"

namespace NN::Components {
  struct Position;
}

namespace NN::Utils {
  struct RayCastHit {
    int mapX;
    int mapY;
    double wallX;
    int texX;
    int side;
    double perpWallDist;
    int colorIndex;
    int textureIndex;
    // Door info
    bool isDoor;
    bool doorOpensUp;
    double doorOpenProgress;
  };

  struct EntityHit {
    Entities::Entity entity;
    double distance;
    double hitX;
    double hitY;
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

      RayCastHit collideWorld(World *world);

      // Test ray against a circle at (cx, cy) with given radius.
      // Returns distance along ray to hit point, or -1 if no hit.
      double intersectCircle(double cx, double cy, double radius) const;
  };
}
#endif
