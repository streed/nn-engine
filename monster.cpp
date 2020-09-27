#include <iostream>
#include <cmath>
#include <queue>
#include <map>
#include <limits>

using namespace std;

#include "monster.h"
#include "player.h"

void Monster::update(World &world, Player *player, std::vector<Entity *> *entities, double timeDiff) {

  double distanceToPlayer = (posX - player->posX) * (posX - player->posX) +
                            (posY - player->posY) * (posY - player->posY);

  if (distanceToPlayer > searchDistance) {
    seeking = true;
  } else {
    seeking = false;
  }

  if (seeking) {
    Point nextCellToMoveTo = findNextCellToMoveTo(world, player);
    /*double diffX = posX - nextCellToMoveTo.first;
    double diffY = posY - nextCellToMoveTo.second;
    double length = sqrt(diffX * diffX + diffY * diffY);

    double dirX = diffX / length;
    double dirY = diffY / length;

    posX = posX + maxSpeedClip * timeDiff;
    posY = posY + maxSpeedClip * timeDiff;*/
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
    S.push_back(u);
    auto search = previous.find(u);
    if (search != previous.end()) {
      u = search->second;
    } else {
      break;
    }
  }

  if (S.size() >= 2) {
    return S.at(1);
  } else {
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
      if(world.getMapPoint(px, py) == 0) {
        neighbors.push_back(point);
      }
    }
  }

  return neighbors;
}
