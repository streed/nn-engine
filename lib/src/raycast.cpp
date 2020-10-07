#include "raycast.h"

#include <math.h>

#include "globals.h"
namespace NN::Utils {
  RayCastHit RayCast::collideWorld(World *world) {
    double sideDistX;
    double sideDistY;

    double deltaDistX = fabs(1 / dirX);
    double deltaDistY = fabs(1 / dirY);

    int stepX;
    int stepY;
    int hit = 0;
    int side;

    int mapX = int(startX);
    int mapY = int(startY);

    if (dirX < 0) {
      stepX = -1;
      sideDistX = (startX - mapX) * deltaDistX;
    } else {
      stepX = 1;
      sideDistX = (mapX + 1.0 - startX) * deltaDistX;
    }

    if (dirY < 0) {
      stepY = -1;
      sideDistY = (startY - mapY) * deltaDistY;
    } else {
      stepY = 1;
      sideDistY = (mapY + 1.0 - startY) * deltaDistY;
    }

    while(hit == 0) {
      if (sideDistX < sideDistY) {
        sideDistX += deltaDistX;
        mapX += stepX;
        side = 0;
      } else {
        sideDistY += deltaDistY;
        mapY += stepY;
        side = 1;
      }

      if (world->getMapPoint(mapX, mapY) > 0) {
        hit = 1;
      }
    }

    double perpWallDist;
    double wallX;
    if (side == 0) {
      perpWallDist = (mapX - startX + (1 - stepX) / 2) / dirX;
      wallX = startY + perpWallDist * dirY;
    } else {
      perpWallDist = (mapY - startY + (1 - stepY) / 2) / dirY;
      wallX = startX + perpWallDist * dirX;
    }

    wallX -= floor(wallX);

    int texX = int(wallX * double(TEXTURE_WIDTH));
    if (side == 0 && dirX > 0) {
      texX = TEXTURE_WIDTH - texX - 1;
    }

    if (side == 1 && dirY < 0) {
      texX = TEXTURE_WIDTH - texX - 1;
    }

    return {
      mapX,
      mapY,
      wallX,
      texX,
      side,
      perpWallDist,
      world->getMapPoint(mapX, mapY),
      world->getMapPoint(mapX, mapY) - 1
    };
  }
}
