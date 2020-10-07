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
  return *(this->map + x * this->height + y);
}

void World::markNotTraversable(int x, int y) {
  navMesh[x * this->height + y] = false;
}

void World::markTraversable(int x, int y) {
  navMesh[x * this->height + y] = true;
}

bool World::isTraversable(int x, int y) {
  return navMesh[x * this->height + y];
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
      navMesh[i * this->height + j] = getMapPoint(i, j) == 0 ? true: false;
    }
  }
}

void World::draw() {
  cout << "NavMesh: " << endl;
  for (int i =  0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      cout << navMesh[i * this->width + j] << ",";
    }

    cout << "\t";

    for (int j = 0; j < width; j++) {
      cout << getMapPoint(i, j) << ",";
    }

    cout << endl;
  }
}
