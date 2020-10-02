#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

class World {
  public:
    int width;
    int height;
    int *map;
    bool **navMesh;

    World(int width, int height, int *map);
    ~World();

    int getMapPoint(int x, int y);

    void markNotTraversable(int x, int y);
    void markTraversable(int x, int y);
    bool isTraversable(int x, int y);
    void resetNavMesh();
    void draw();

  private:
    void setupNavMesh();
};

#endif
