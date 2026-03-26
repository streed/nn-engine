#include <iostream>
#include <cmath>

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
  if (x < 0 || x >= width || y < 0 || y >= height) {
    return 1; // treat out-of-bounds as solid wall
  }
  return *(this->map + y * this->width + x);
}

void World::setMapPoint(int x, int y, int value) {
  if (x < 0 || x >= width || y < 0 || y >= height) return;
  *(this->map + y * this->width + x) = value;
  // Update navmesh: 0 = traversable, anything else = solid
  if (value == 0) {
    markTraversable(x, y);
  } else {
    markNotTraversable(x, y);
  }
}

void World::markNotTraversable(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= height) return;
  navMesh[y * this->width + x] = false;
}

void World::markTraversable(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= height) return;
  navMesh[y * this->width + x] = true;
}

bool World::isTraversable(int x, int y) {
  if (x < 0 || x >= width || y < 0 || y >= height) return false;
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
  std::cout << "NavMesh: " << std::endl;
  for (int i =  0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      std::cout << navMesh[i * width  + j] << ",";
    }

    std::cout << "\t";

    for (int j = 0; j < width; j++) {
      std::cout << getMapPoint(j, i) << ",";
    }

    std::cout << std::endl;
  }
}

// Door management

std::int64_t World::doorKey(int x, int y) const {
  return (static_cast<std::int64_t>(y) << 32) | static_cast<std::int64_t>(x);
}

void World::addDoor(int x, int y, bool opensUp, int textureIndex, double speed, double autoCloseDelay) {
  DoorState door{};
  door.mapX = x;
  door.mapY = y;
  door.opensUp = opensUp;
  door.textureIndex = textureIndex;
  door.openProgress = 0.0;
  door.openTime = 0.0;
  door.speed = speed;
  door.opening = false;
  door.closing = false;
  door.autoCloseTimer = -1.0;
  door.autoCloseDelay = autoCloseDelay;

  size_t index = doors.size();
  doors.push_back(door);
  doorLookup[doorKey(x, y)] = index;

  // Door tiles are solid walls in the map, so navMesh should block them
  markNotTraversable(x, y);
}

DoorState *World::getDoor(int x, int y) {
  auto it = doorLookup.find(doorKey(x, y));
  if (it != doorLookup.end()) {
    return &doors[it->second];
  }
  return nullptr;
}

bool World::hasDoor(int x, int y) const {
  return doorLookup.find(doorKey(x, y)) != doorLookup.end();
}

const std::vector<DoorState> &World::getDoors() const {
  return doors;
}

void World::updateDoors(double frameTime) {
  static constexpr double PASSABLE_THRESHOLD = 0.7;

  for (auto &door : doors) {
    if (door.opening) {
      door.openTime += door.speed * frameTime;
      if (door.openTime >= 1.0) {
        door.openTime = 1.0;
        door.opening = false;
        // Start auto-close timer
        if (door.autoCloseDelay > 0) {
          door.autoCloseTimer = door.autoCloseDelay;
        }
      }
    } else if (door.closing) {
      door.openTime -= door.speed * frameTime;
      if (door.openTime <= 0.0) {
        door.openTime = 0.0;
        door.closing = false;
      }
    } else if (door.autoCloseTimer > 0) {
      // Count down auto-close timer
      door.autoCloseTimer -= frameTime;
      if (door.autoCloseTimer <= 0) {
        door.autoCloseTimer = -1.0;
        door.closing = true;
      }
    }

    // Smoothstep easing: slow start, fast middle, slow end
    double t = door.openTime;
    door.openProgress = t * t * (3.0 - 2.0 * t);

    // Update navmesh based on door state
    if (door.openProgress >= PASSABLE_THRESHOLD) {
      markTraversable(door.mapX, door.mapY);
    } else {
      markNotTraversable(door.mapX, door.mapY);
    }
  }
}

bool World::tryInteractDoor(double playerX, double playerY, double playerDirX, double playerDirY) {
  static constexpr double INTERACT_RANGE = 2.5;

  // Check tiles in front of the player
  for (double dist = 0.5; dist <= INTERACT_RANGE; dist += 0.5) {
    int checkX = static_cast<int>(playerX + playerDirX * dist);
    int checkY = static_cast<int>(playerY + playerDirY * dist);

    DoorState *door = getDoor(checkX, checkY);
    if (door) {
      if (!door->opening && !door->closing) {
        if (door->openProgress < 0.5) {
          door->opening = true;
          door->closing = false;
          door->autoCloseTimer = -1.0;
        } else {
          door->closing = true;
          door->opening = false;
          door->autoCloseTimer = -1.0;
        }
      }
      return true;
    }
  }
  return false;
}

// Zone management

void World::addZone(const std::string &name, double x, double y, double w, double h) {
  zones.push_back({name, x, y, w, h});
}

bool World::isInZone(const std::string &name, double px, double py) const {
  for (const auto &zone : zones) {
    if (zone.name == name) {
      return px >= zone.x && px < zone.x + zone.w &&
             py >= zone.y && py < zone.y + zone.h;
    }
  }
  return false;
}

void World::updateZones(double px, double py,
                        std::vector<std::string> &entered,
                        std::vector<std::string> &exited) {
  entered.clear();
  exited.clear();

  std::unordered_set<std::string> nowIn;
  for (const auto &zone : zones) {
    if (px >= zone.x && px < zone.x + zone.w &&
        py >= zone.y && py < zone.y + zone.h) {
      nowIn.insert(zone.name);
    }
  }

  // Newly entered zones
  for (const auto &name : nowIn) {
    if (activeZones.find(name) == activeZones.end()) {
      entered.push_back(name);
    }
  }

  // Exited zones
  for (const auto &name : activeZones) {
    if (nowIn.find(name) == nowIn.end()) {
      exited.push_back(name);
    }
  }

  activeZones = nowIn;
}

const std::vector<Zone> &World::getZones() const {
  return zones;
}
