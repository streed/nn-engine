#include <math.h>

#include "player.h"

Player::Player(Camera *camera,
               double posX,
               double posY,
               double accelerationConstant,
               double maxSpeedClip,
               double maxRotateSpeedClip): MovingEntity(posX,
                                                  posY,
                                                  accelerationConstant,
                                                  maxSpeedClip,
                                                  maxRotateSpeedClip),
                                            camera(camera) {}

int Player::getMapX() {
  return int(this->posX);
}

int Player::getMapY() {
  return int(this->posY);
}

void Player::handleInputs(InputPacket inputPacket, World world, double frameTime) {
  double moveSpeed = this->maxSpeedClip * frameTime;
  double rotateSpeed = this->maxRotateSpeedClip * frameTime;

  if (inputPacket.forward) {
    double newX = this->posX + this->camera->dirX * moveSpeed;
    double newY = this->posY + this->camera->dirY * moveSpeed;

    if (world.getMapPoint(int(newX), this->posY) == 0) {
      this->posX = newX;
    }

    if (world.getMapPoint(this->posX, int(newY)) == 0) {
      this->posY = newY;
    }
  }

  if (inputPacket.backward) {
    double newX = this->posX - this->camera->dirX * moveSpeed;
    double newY = this->posY - this->camera->dirY * moveSpeed;

    if (world.getMapPoint(int(newX), this->posY) == 0) {
      this->posX = newX;
    }

    if (world.getMapPoint(this->posX, int(newY)) == 0) {
      this->posY = newY;
    }
  }

  if (inputPacket.strafeLeft) {
    double perpDirX = this->camera->dirY;
    double perpDirY = -this->camera->dirX;

    double newX = this->posX - perpDirX * moveSpeed;
    double newY = this->posY - perpDirY * moveSpeed;

    if (world.getMapPoint(int(newX), this->posY) == 0) {
      this->posX = newX;
    }

    if (world.getMapPoint(this->posX, int(newY)) == 0) {
      this->posY = newY;
    }
  }

  if (inputPacket.strafeRight) {
    double perpDirX = this->camera->dirY;
    double perpDirY = -this->camera->dirX;

    double newX = this->posX + perpDirX * moveSpeed;
    double newY = this->posY + perpDirY * moveSpeed;

    if (world.getMapPoint(int(newX), this->posY) == 0) {
      this->posX = newX;
    }

    if (world.getMapPoint(this->posX, int(newY)) == 0) {
      this->posY = newY;
    }
  }

  if (inputPacket.rotateRight) {
    double oldDirX = this->camera->dirX;
    this->camera->dirX = this->camera->dirX * cos(-rotateSpeed) - this->camera->dirY * sin(-rotateSpeed);
    this->camera->dirY = oldDirX * sin(-rotateSpeed) + this->camera->dirY * cos(-rotateSpeed);
    double oldPlaneX = this->camera->planeX;
    this->camera->planeX = this->camera->planeX * cos(-rotateSpeed) - this->camera->planeY * sin(-rotateSpeed);
    this->camera->planeY = oldPlaneX * sin(-rotateSpeed) + this->camera->planeY * cos(-rotateSpeed);
  }

  if (inputPacket.rotateLeft) {
    double oldDirX = this->camera->dirX;
    this->camera->dirX = this->camera->dirX * cos(rotateSpeed) - this->camera->dirY * sin(rotateSpeed);
    this->camera->dirY = oldDirX * sin(rotateSpeed) + this->camera->dirY * cos(rotateSpeed);
    double oldPlaneX = this->camera->planeX;
    this->camera->planeX = this->camera->planeX * cos(rotateSpeed) - this->camera->planeY * sin(rotateSpeed);
    this->camera->planeY = oldPlaneX * sin(rotateSpeed) + this->camera->planeY * cos(rotateSpeed);
  }
}
