#include <iostream>
using namespace std;

#include "world.h"

World::World(int width, int height, int *map) {
  this->width = width;
  this->height = height;
  this->map = map;

  setupNavMesh();
}

World::~World() {
  delete [] navMesh;
}

int World::getMapPoint(int x, int y) {
  return *(this->map + y * this->width + x);
}

void World::markNotTraversable(int x, int y) {
  navMesh[y * this->width+ x] = false;
}

void World::markTraversable(int x, int y) {
  navMesh[y * this->width+ x] = true;
}

bool World::isTraversable(int x, int y) {
  return navMesh[y * this->width + x];
}

void World::setupNavMesh() {
  navMesh = new bool[height*width];

  for (int i = 0; i < height * width; i++) {
      navMesh[i] = false;
  }

  resetNavMesh();

}

void World::resetNavMesh() {
  for (int i =  0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      navMesh[i * this->width + j] = getMapPoint(j, i) == 0 ? true: false;
    }
  }
}

void World::draw() {
  cout << "NavMesh: " << endl;
  for (int i =  0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      cout << navMesh[i * width  + j] << ",";
    }

    cout << "\t";

    for (int j = 0; j < width; j++) {
      cout << getMapPoint(j, i) << ",";
    }

    cout << endl;
  }
}
