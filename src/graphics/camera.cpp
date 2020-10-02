#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "globals.h"
#include "graphics/camera.h"

Camera::Camera(double dirX, double dirY) {
  this->dirX = dirX;
  this->dirY = dirY;
  //  FOV: Set 2 * atan(0.66 / 1 ) == 66 degrees
  this->planeX = 0.66 * this->dirY;
  this->planeY = 0.66 * -this->dirX;
};

Camera::Camera(double dirX, double dirY, double planeX, double planeY) {
  this->dirX = dirX;
  this->dirY = dirY;
  this->planeX = planeX;
  this->planeY = planeY;
}

double Camera::getCameraX(int x) {
  return  2 * x / double(SCREEN_WIDTH) - 1;
}

double Camera::getRayDirX(double cameraX) {
  return this->dirX + this->planeX * cameraX;
}

double Camera::getRayDirY(double cameraX) {
  return this->dirY + this->planeY * cameraX;
}

double Camera::getInvDet() {
  return 1.0 / (planeX * dirY - dirX * planeY);
}

void Camera::rotate(const double rotateSpeed) {
  double oldDirX = dirX;
  dirX = dirX * cos(rotateSpeed) - dirY * sin(rotateSpeed);
  dirY = oldDirX * sin(rotateSpeed) + dirY * cos(rotateSpeed);
  double oldPlaneX = planeX;
  planeX = planeX * cos(rotateSpeed) - planeY * sin(rotateSpeed);
  planeY = oldPlaneX * sin(rotateSpeed) + planeY * cos(rotateSpeed);
}
