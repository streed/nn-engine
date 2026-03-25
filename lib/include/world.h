#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include <unordered_map>
#include <vector>
#include <cstdint>

struct DoorState {
  int mapX, mapY;
  bool opensUp;          // true = slides up into ceiling, false = slides down into floor
  int textureIndex;      // texture to render the door with
  double openProgress;   // 0.0 = fully closed, 1.0 = fully open
  double speed;          // how fast the door opens/closes (progress per second)
  bool opening;
  bool closing;
  double autoCloseTimer; // seconds until door starts closing (-1 = no auto-close)
  double autoCloseDelay; // configured delay before auto-close
};

class World {
  public:
    int width;
    int height;
    int *map;
    bool *navMesh;

    World(int width, int height, int *map);
    ~World();

    int getMapPoint(int x, int y);

    void markNotTraversable(int x, int y);
    void markTraversable(int x, int y);
    bool isTraversable(int x, int y);
    void resetNavMesh();
    void draw();

    // Door management
    void addDoor(int x, int y, bool opensUp, int textureIndex, double speed = 2.0, double autoCloseDelay = 5.0);
    DoorState *getDoor(int x, int y);
    bool hasDoor(int x, int y) const;
    void updateDoors(double frameTime);
    bool tryInteractDoor(double playerX, double playerY, double playerDirX, double playerDirY);
    const std::vector<DoorState> &getDoors() const;

  private:
    void setupNavMesh();
    std::int64_t doorKey(int x, int y) const;

    std::vector<DoorState> doors;
    std::unordered_map<std::int64_t, size_t> doorLookup; // key -> index in doors vector
};

#endif
