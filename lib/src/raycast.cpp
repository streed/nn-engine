#include "raycast.h"

#include <cmath>

#include "globals.h"
namespace NN::Utils {
  RayCastHit RayCast::collideWorld(World *world) {
    double sideDistX;
    double sideDistY;

    double deltaDistX = (dirX == 0) ? 1e30 : fabs(1 / dirX);
    double deltaDistY = (dirY == 0) ? 1e30 : fabs(1 / dirY);

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
        // Check if this is a fully open door - if so, skip it
        DoorState *door = world->getDoor(mapX, mapY);
        if (door && door->openProgress >= 1.0) {
          continue; // Ray passes through open door
        }
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

    // Check if the hit tile is a door
    DoorState *door = world->getDoor(mapX, mapY);
    bool isDoor = door != nullptr;
    bool doorOpensUp = isDoor ? door->opensUp : false;
    double doorOpenProgress = isDoor ? door->openProgress : 0.0;
    int textureIndex = isDoor ? door->textureIndex : (world->getMapPoint(mapX, mapY) - 1);

    return {
      mapX,
      mapY,
      wallX,
      texX,
      side,
      perpWallDist,
      world->getMapPoint(mapX, mapY),
      textureIndex,
      isDoor,
      doorOpensUp,
      doorOpenProgress
    };
  }

  double RayCast::intersectCircle(double cx, double cy, double radius) const {
    // Vector from ray origin to circle center
    double ocX = cx - startX;
    double ocY = cy - startY;

    // Project onto ray direction
    double dot = ocX * dirX + ocY * dirY;
    if (dot < 0.0) return -1.0; // Circle is behind ray

    // Closest point on ray to circle center
    double closestX = startX + dirX * dot;
    double closestY = startY + dirY * dot;

    double distSq = (closestX - cx) * (closestX - cx) + (closestY - cy) * (closestY - cy);
    double radiusSq = radius * radius;

    if (distSq > radiusSq) return -1.0; // Ray misses

    // Distance from closest point back to intersection
    double offset = std::sqrt(radiusSq - distSq);
    double t = dot - offset;

    return (t >= 0.0) ? t : dot + offset;
  }
}
