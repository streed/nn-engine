#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include <string>
#include <unordered_map>
#include <unordered_set>
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

struct Zone {
  std::string name;
  double x, y, w, h;
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
    void setMapPoint(int x, int y, int value);

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

    // Zone management
    void addZone(const std::string &name, double x, double y, double w, double h);
    bool isInZone(const std::string &name, double px, double py) const;
    void updateZones(double px, double py,
                     std::vector<std::string> &entered,
                     std::vector<std::string> &exited);
    const std::vector<Zone> &getZones() const;

  private:
    void setupNavMesh();
    std::int64_t doorKey(int x, int y) const;

    std::vector<DoorState> doors;
    std::unordered_map<std::int64_t, size_t> doorLookup;

    std::vector<Zone> zones;
    std::unordered_set<std::string> activeZones; // zones the player is currently in
};

#endif
