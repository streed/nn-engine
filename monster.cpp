#include <iostream>
#include <cmath>
#include <queue>
#include <map>
#include <limits>

using namespace std;

#include "monster.h"
#include "player.h"

// Taken from Quake3 source code.
float inverseSqrt(float number) {
  const float threeHalfs = 1.5F;

  float x2 = number * 0.5F;
  float y = number;

  long i = *(long *)&y;

  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (threeHalfs - (x2 * y * y));
  return y;
}

void Monster::update(World &world, Player *player, std::vector<Entity *> *entities, double timeDiff) {

  float distanceToPlayer = (posX - player->posX) * (posX - player->posX) +
                            (posY - player->posY) * (posY - player->posY);
  distanceToPlayer *= inverseSqrt(distanceToPlayer);

  if (distanceToPlayer > searchDistance) {
    seeking = true;
  } else {
    seeking = false;
  }

  if (seeking) {
    double moveSpeed = maxSpeedClip * timeDiff;
    Point nextCellToMoveTo = findNextCellToMoveTo(world, player);
    float diffX = 1.0 * nextCellToMoveTo.first - posX;
    float diffY = 1.0 * nextCellToMoveTo.second - posY;
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
  }
}

Point Monster::findNextCellToMoveTo(World &world, Player *player) {
  int myX = int(posX);
  int myY = int(posY);
  int cellX = int(player->posX);
  int cellY = int(player->posY);

  Point source(myX, myY);
  Point destination(cellX, cellY);

  std::map<Point, Point> Q;
  std::map<Point, int> distances;
  std::map<Point, Point> previous;

  Point undefined(-1, -1);
  for(int x = 0; x < world.width; x++) {
    for(int y = 0; y < world.height; y++) {
      if(world.getMapPoint(x,y) == 0) {
        Point point(x,y);
        distances[point] = std::numeric_limits<int>::max();
        previous[point] = undefined;
        Q[point] = point;
      }
    }
  }

  distances[source] = 0;

  while(!Q.empty()) {
    std::vector<Point> remaining; 

    for(std::pair<Point, Point> const &p: Q) {
      remaining.push_back(p.first);
    }

    Point shortest = remaining.at(0);

    // We made it!
    if (shortest == destination) {
      break;
    }

    for(auto const &point: remaining) {
      if (distances[shortest] > distances[point]) {
        shortest = point;
      }
    }

    Q.erase(shortest);

    std::vector<Point> neighbors = getNeighbors(world, shortest, Q);

    for (auto const &v: neighbors) {
      int alt = distances[shortest] +
                (shortest.first - v.first) * (shortest.first - v.first) +
                (shortest.second - v.second) * (shortest.second - v.second);

      if (alt < distances[v]) {
        distances[v] = alt;
        previous[v] = shortest;
      }
    }
  }

  std::vector<Point> S;
  Point u = destination;

  while (true) {
    if (u != source) {
      S.insert(S.begin(), u);
    }
    auto search = previous.find(u);
    if (search != previous.end() && search->second != undefined) {
      u = search->second;
    } else {
      break;
    }
  }

  if (S.size() >= 1) {
    return S.front();
  } else {
    cout << source.first << " " << source.second << endl;
    return source;
  }
}

std::vector<Point> Monster::getNeighbors(World &world, Point point, std::map<Point, Point> Q) {
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
      if(world.getMapPoint(px, py) == 0 && Q.count(point) > 0) {
        neighbors.push_back(point);
      }
    }
  }

  return neighbors;
}
