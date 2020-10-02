#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <queue>

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

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

Components::Registration<ImpAIComponent> ImpAIComponent::registered = Components::Registration<ImpAIComponent>("ImpAIComponent");

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
    Point nextCellToMoveTo = findNextCellToMoveTo(imp, player, world, engine);

    if (nextCellToMoveTo.first == int(player.posX) && nextCellToMoveTo.second == int(player.posY)) {
      imp->velocityX = 0;
      imp->velocityY = 0;
      return;
    }

    double moveSpeed = imp->maxSpeedClip * timeDiff;
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
  int diffX = a.first - b.first;
  int diffY = a.second - b.second;
  return diffX * diffX + diffY * diffY;
}

int h(Point p, Point d) {
  return distance(p, d);
}

Point getNextFromOpenSet(std::map<Point, Point> openSet, std::map<Point, unsigned int> fScore) {
  std::vector<std::pair<unsigned int, Point> > fScorePointTuples;

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

  world.markTraversable(cellX, cellY);

  Point source(myX, myY);
  Point destination(cellX, cellY);

  std::map<Point, Point> openSet;
  std::map<Point, Point> cameFrom;

  std::map<Point, unsigned int> gScore;
  std::map<Point, unsigned int> fScore;

  for(int x = 0; x < world.width; x++) {
    for(int y = 0; y < world.height; y++) {
      if(world.isTraversable(x,y)) {
        Point point(x, y);
        gScore[point] = std::numeric_limits<unsigned int>::max();
        fScore[point] = std::numeric_limits<unsigned int>::max();
      }
    }
  }

  gScore[source] = 0;
  fScore[source] = h(source, destination);

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
      unsigned int tentativeGScore = gScore[current] + 1;

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
    if (engine.getDebug()) {
      char **pathMap = new char*[world.height];
      for (int i = 0; i < world.height; i++) {
        pathMap[i] = new char[world.width];
        for (int j = 0; j < world.width; j++) {
          pathMap[i][j] = '*';
          pathMap[i][j] = world.navMesh[i][j] ? '_' : '%';
        }
      }

      for (auto &p: path) {
        pathMap[p.first][p.second] = '+';
      }

      pathMap[path.front().first][path.front().second] = 'O';
      pathMap[path.back().first][path.back().second] = 'X';


      cout << "Path: " << endl;
      for (int i =  0; i < world.height; i++) {
        for (int j = 0; j < world.width; j++) {
          cout << pathMap[i][j] << ",";
        }

        for (int j = 0; j < world.width; j++) {
          cout << world.navMesh[i][j] << ",";
        }

        for (int j = 0; j < world.width; j++) {
          cout << world.map[j + world.height * i] << ",";
        }

        cout << endl;
      }

      delete [] pathMap;
    }

    if (path.size() >= 2) {
      return path.at(1);
    } else {
      return path.at(0);
    }
  } else {
    return source;
  }
}

std::vector<Point> getNeighbors(World &world, Engine &engine, Point point) {
  int x = point.first;
  int y = point.second;
  int boundX = world.width;
  int boundY = world.height;
  std::vector<Point> potentialNeighbors;

  potentialNeighbors.push_back(Point(x,     y - 1));
  potentialNeighbors.push_back(Point(x,     y + 1));

  potentialNeighbors.push_back(Point(x - 1, y    ));
  potentialNeighbors.push_back(Point(x + 1, y    ));

  potentialNeighbors.push_back(Point(x - 1, y + 1));
  potentialNeighbors.push_back(Point(x + 1, y + 1));

  potentialNeighbors.push_back(Point(x - 1, y - 1));
  potentialNeighbors.push_back(Point(x + 1, y - 1));

  std::vector<Point> neighbors;

  for(auto const &potentialPoint: potentialNeighbors) {
    int px = potentialPoint.first;
    int py = potentialPoint.second;

    bool traversable = world.isTraversable(px, py);
    if (!((px < 0 || py < 0 || px >= boundX || py >= boundY) && !traversable)) {
      neighbors.push_back(Point(px, py));
    }
  }

  return neighbors;
}


