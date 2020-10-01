#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <queue>

#include <SDL2/SDL.h>

typedef std::pair<int, int> Point;
using namespace std;

#include "components/imp_ai_component.h"
#include "components/projectile_physics_component.h"
#include "components/sprite_graphics_component.h"

#include "game_objects/player.h"
#include "game_objects/imp.h"
#include "game_objects/projectile_object.h"
#include "world.h"
#include "raycast.h"
#include "utils.h"
#include "engine.h"

int distance(Point a, Point b);
int h(Point p, Point d);
Point getNextFromOpenSet(std::map<Point, Point> openSet, std::map<Point, int> fScore);
std::vector<Point> reconstructPath(std::map<Point, Point> cameFrom, Point start, Point end);
Point findNextCellToMoveTo(Imp *imp, Player &player, World &world, Engine &engine);
std::vector<Point> getNeighbors(World &world, Engine &engine, Point point);

void ImpAIComponent::update(GameObject *object, Engine &engine, World &world, double timeDiff) {
  Imp *imp = dynamic_cast<Imp *>(object);
  Player &player = *engine.getPlayer();
  float distanceToPlayer = (imp->posX - player.posX) * (imp->posX - player.posX) +
                            (imp->posY - player.posY) * (imp->posY - player.posY);
  distanceToPlayer *= inverseSqrt(distanceToPlayer);

  if (distanceToPlayer > searchDistance) {
    seeking = true;
  } else {
    seeking = false;
  }

  imp->velocityX = 0;
  imp->velocityY = 0;

  // Move close to the player!
  if (seeking) {
    double moveSpeed = imp->maxSpeedClip * timeDiff;
    Point nextCellToMoveTo = findNextCellToMoveTo(imp, player, world, engine);
    float diffX = 1.0 * (nextCellToMoveTo.first + 0.5 - imp->posX);
    float diffY = 1.0 * (nextCellToMoveTo.second + 0.5 - imp->posY);
    float length = diffX * diffX + diffY * diffY;
    float fiSqrt = inverseSqrt(length);

    double dirX = diffX * fiSqrt;
    double dirY = diffY * fiSqrt;

    double velocityX = dirX * moveSpeed;
    double velocityY = dirY * moveSpeed;
    imp->velocityX = velocityX;
    imp->velocityY = velocityY;
  } else { // Shoot at the player
    if (timeUntilNextShot <= 0.0) {
      timeUntilNextShot = shootingCoolDownConstant;
      float diffX = imp->posX - player.posX;
      float diffY = imp->posY - player.posY;
      float length = (diffX * diffX) + (diffY * diffY);
      float fiSqrt = inverseSqrt(length);

      double projectileDirX = -diffX * fiSqrt;
      double projectileDirY = -diffY * fiSqrt;

      RayCast ray(imp->posX, imp->posY, projectileDirX, projectileDirY);
      RayCastHit hit = ray.collideWorld(world);
      float distanceToWall = (imp->posX - hit.mapX) * (imp->posX - hit.mapX) +
                             (imp->posY - hit.mapY) * (imp->posY - hit.mapY);
      distanceToWall *= inverseSqrt(distanceToWall);

      if (distanceToPlayer < distanceToWall) {
        ProjectileObject *projectile = new ProjectileObject(imp->posX,
                                                            imp->posY,
                                                            projectileDirX,
                                                            projectileDirY,
                                                            2,
                                                            3.5,
                                                            new ProjectilePhysicsComponent(),
                                                            new SpriteGraphicsComponent(new Sprite(12)));
        engine.addGameObject(projectile);
      }
    } else {
      timeUntilNextShot -= timeDiff;
    }
  }
}

int distance(Point a, Point b) {
  return (a.first - b.first) * (a.first - b.first) +
         (a.second - b.second) * (a.second - b.second);
}

int h(Point p, Point d) {
  return distance(p, d);
}

Point getNextFromOpenSet(std::map<Point, Point> openSet, std::map<Point, int> fScore) {
  std::vector<std::pair<int, Point> > fScorePointTuples;

  for (auto const &point: openSet) {
    int f = fScore[point.first];
    fScorePointTuples.push_back(std::make_pair(f, point.first));
  }

  std::sort(fScorePointTuples.begin(), fScorePointTuples.end());

  return fScorePointTuples.front().second;
}

std::vector<Point> reconstructPath(std::map<Point, Point> cameFrom, Point start, Point end) {
  std::vector<Point> path;
  path.push_back(end);

  while (end != start) {
    end = cameFrom[end];
    path.insert(path.begin(), end);
  }

  return path;
}

Point findNextCellToMoveTo(Imp *imp, Player &player, World &world, Engine &engine) {
  int myX = int(imp->posX);
  int myY = int(imp->posY);
  int cellX = int(player.posX);
  int cellY = int(player.posY);

  Point source(myX, myY);
  Point destination(cellX, cellY);

  std::map<Point, Point> openSet;
  std::map<Point, Point> cameFrom;

  std::map<Point, int> gScore;
  std::map<Point, int> fScore;

  for(int x = 0; x < world.width; x++) {
    for(int y = 0; y < world.height; y++) {
      if(world.getMapPoint(x,y) == 0) {
        Point point(x,y);
        gScore[point] = std::numeric_limits<int>::max();
        fScore[point] = std::numeric_limits<int>::max();
      }
    }
  }

  bool foundPath = false;
  openSet[source] = source;
  Point current;
  while (!openSet.empty()) {
    current = getNextFromOpenSet(openSet, fScore);

    if (current == destination) {
      foundPath = true;
      break;
    }

    openSet.erase(current);

    for (auto const &neighbor: getNeighbors(world, engine, current)) {
      int tentativeGScore = gScore[current] + distance(current, neighbor);

      if (tentativeGScore < gScore[neighbor]) {
        cameFrom[neighbor] = current;
        gScore[neighbor] = tentativeGScore;
        fScore[neighbor] = gScore[neighbor] + h(neighbor, destination);

        if (openSet.count(neighbor) == 0) {
          openSet[neighbor] = neighbor;
        }
      }
    }
  }

  if (foundPath) {
    std::vector<Point> path = reconstructPath(cameFrom, source, current);
    return path.at(1);
  } else {
    return source;
  }
}

bool collidedWithOtherObject(std::vector<GameObject *> *objects, Point potentialPoint) {
  int x = potentialPoint.first;
  int y = potentialPoint.second;
  bool collision = false;

  for (auto *object: *objects) {
    PositionalObject *positionalObject = dynamic_cast<PositionalObject *>(object);

    if (positionalObject) {
      int mapX = int(positionalObject->posX);
      int mapY = int(positionalObject->posY);

      if (x == mapX && y == mapY) {
        collision = true;
        break;
      }
    }
  }

  return collision;
}

std::vector<Point> getNeighbors(World &world, Engine &engine, Point point) {
  int x = point.first;
  int y= point.second;
  int boundX = world.width;
  int boundY = world.height;
  std::vector<Point> potentialNeighbors;

  potentialNeighbors.push_back(Point(x,     y - 1));
  potentialNeighbors.push_back(Point(x - 1, y    ));
  potentialNeighbors.push_back(Point(x + 1, y    ));
  potentialNeighbors.push_back(Point(x,     y + 1));

  std::vector<Point> neighbors;
  std::vector<GameObject *> *objects = engine.getGameObjects();

  for(auto const &potentialPoint: potentialNeighbors) {
    int px = potentialPoint.first;
    int py = potentialPoint.second;

    // TODO: Fix collision
    //bool collision = collidedWithOtherObject(objects, potentialPoint);
    int worldIndex = world.getMapPoint(px, py);
    if ((px < 0 || py < 0 || px >= boundX || py >= boundY) || worldIndex != 0 /*|| collison*/) {
      continue;
    } else {
      neighbors.push_back(Point(px, py));
    }
  }

  return neighbors;
}


