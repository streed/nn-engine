#include "world.h"

World::World(int width, int height, int *map) {
  this->width = width;
  this->height = height;
  this->map = map;
}

int World::getMapPoint(int x, int y) {
  return *(this->map + x * this->height + y);
}
