#ifndef CAMERA_H
#define CAMERA_H

class Camera {
  public:
    double dirX;
    double dirY;
    double planeX;
    double planeY;

    Camera(double dirX, double dirY, double planeX, double planeY);
    double getCameraX(int x);
    double getRayDirX(double cameraX);
    double getRayDirY(double cameraX);
};

#endif
