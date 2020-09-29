#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

class World {
  public:
    int width;
    int height;
    int *map;

    World(int width, int height, int *map);
    int getMapPoint(int x, int y);
};

#endif
