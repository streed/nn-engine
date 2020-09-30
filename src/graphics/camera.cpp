#include <SDL2/SDL.h>

#include "globals.h"
#include "graphics/camera.h"

double Camera::getCameraX(int x) {
  return  2 * x / double(SCREEN_WIDTH) - 1;
}

double Camera::getRayDirX(double cameraX) {
  return this->dirX + this->planeX * cameraX;
}

double Camera::getRayDirY(double cameraX) {
  return this->dirY + this->planeY * cameraX;
}

Camera::Camera(double dirX, double dirY, double planeX, double planeY) {
  this->dirX = dirX;
  this->dirY = dirY;
  this->planeX = planeX;
  this->planeY = planeY;
}

double Camera::getInvDet() {
  return 1.0 / (planeX * dirY - dirX * planeY);
}
