#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <queue>

#include <SDL2/SDL.h>

using namespace std;

#include "monster.h"
#include "player.h"
#include "projectile.h"
#include "game.h"
#include "utils.h"


void Monster::update(Game *game,
                     World &world,
                     Player *player,
                     std::vector<Entity *> *entities,
                     double timeDiff) {

  float distanceToPlayer = (posX - player->posX) * (posX - player->posX) +
                            (posY - player->posY) * (posY - player->posY);
  distanceToPlayer *= inverseSqrt(distanceToPlayer);

  if (distanceToPlayer > searchDistance) {
    seeking = true;
  } else {
    seeking = false;
  }

  // Move close to the player!
  if (seeking) {
    double moveSpeed = maxSpeedClip * timeDiff;
    Point nextCellToMoveTo = findNextCellToMoveTo(world, player);
    float diffX = 1.0 * (nextCellToMoveTo.first + 0.5 - posX);
    float diffY = 1.0 * (nextCellToMoveTo.second + 0.5 - posY);
    float length = diffX * diffX + diffY * diffY;
    float fiSqrt = inverseSqrt(length);

    double dirX = diffX * fiSqrt;
    double dirY = diffY * fiSqrt;

    double newPosX = posX + dirX * moveSpeed;
    double newPosY = posY + dirY * moveSpeed;

    if (world.getMapPoint(int(newPosX), int(posY)) == 0) {
      posX = newPosX;
    }

    if (world.getMapPoint(int(posX), int(newPosY)) == 0) {
      posY = newPosY;
    }
  } else { // Shoot at the player
    if (timeUntilNextShot <= 0.0) {
      timeUntilNextShot = shootingCoolDownConstant;
      float diffX = posX - player->posX;
      float diffY = posY - player->posY;
      float length = (diffX * diffX) + (diffY * diffY);
      float fiSqrt = inverseSqrt(length);

      double projectileDirX = -diffX * fiSqrt;
      double projectileDirY = -diffY * fiSqrt;

      Projectile *projectile = new Projectile(posX, posY, projectileDirX, projectileDirY, 2, 3.5, 12);
      game->addEntity(projectile);
    } else {
      timeUntilNextShot -= timeDiff;
    }
  }
}

int distance(Point a, Point b) {
  return (a.first - b.first) * (a.first - b.first) +
         (a.second - b.second) * (a.second - b.second);
}

int h(Point p, Point d, World &world) {
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

Point Monster::findNextCellToMoveTo(World &world, Player *player) {
  int myX = int(posX);
  int myY = int(posY);
  int cellX = int(player->posX);
  int cellY = int(player->posY);

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

    for (auto const &neighbor: getNeighbors(world, current)) {
      int tentativeGScore = gScore[current] + distance(current, neighbor);

      if (tentativeGScore < gScore[neighbor]) {
        cameFrom[neighbor] = current;
        gScore[neighbor] = tentativeGScore;
        fScore[neighbor] = gScore[neighbor] + h(neighbor, destination, world);

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

std::vector<Point> Monster::getNeighbors(World &world, Point point) {
  int x = point.first;
  int y= point.second;
  int boundX = world.width;
  int boundY = world.height;
  std::vector<Point> potentialNeighbors;

  potentialNeighbors.push_back(Point(x - 1, y - 1));
  potentialNeighbors.push_back(Point(x,     y - 1));
  potentialNeighbors.push_back(Point(x + 1, y - 1));
  potentialNeighbors.push_back(Point(x - 1, y));
  potentialNeighbors.push_back(Point(x + 1, y));
  potentialNeighbors.push_back(Point(x - 1, y + 1));
  potentialNeighbors.push_back(Point(x,     y + 1));
  potentialNeighbors.push_back(Point(x + 1, y + 1));

  std::vector<Point> neighbors;

  for(auto const &point: potentialNeighbors) {
    int px = point.first;
    int py = point.second;

    if (px < 0 || py < 0 || px >= boundX || py >= boundY) {
      continue;
    } else {
      if(world.getMapPoint(px, py) == 0) {
        neighbors.push_back(point);
      }
    }
  }

  return neighbors;
}
