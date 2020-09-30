#include <cmath>
#include <boost/scoped_ptr.hpp>

#include "components/player_input_component.h"

#include "game_objects/game_object.h"
#include "game_objects/player.h"
#include "camera.h"
#include "input/keyboard.h"
#include "input/input_packet.h"
#include "utils.h"

void PlayerInputComponent::update(GameObject *object, double frameTime) {
  Player *player = dynamic_cast<Player *>(object);
  boost::scoped_ptr<InputPacket> inputPacket(Keyboard::get().getInput());
  double moveSpeed = player->maxSpeedClip * frameTime;
  double rotateSpeed = player->maxRotateSpeedClip * frameTime;

  double velocityX = 0;
  double velocityY = 0;

  if (inputPacket.get()->forward) {
    velocityX += player->camera->dirX * moveSpeed;
    velocityY += player->camera->dirY * moveSpeed;
  }

  if (inputPacket.get()->backward) {
    velocityX += -player->camera->dirX * moveSpeed;
    velocityY += -player->camera->dirY * moveSpeed;
  }

  if (inputPacket.get()->strafeLeft) {
    double perpDirX = player->camera->dirY;
    double perpDirY = -player->camera->dirX;

    velocityX += -perpDirX * moveSpeed;
    velocityY += -perpDirY * moveSpeed;
  }

  if (inputPacket.get()->strafeRight) {
    double perpDirX = player->camera->dirY;
    double perpDirY = -player->camera->dirX;

    velocityX += perpDirX * moveSpeed;
    velocityY += perpDirY * moveSpeed;
  }

  player->velocityX = velocityX;
  player->velocityY = velocityY;

  if (inputPacket.get()->rotateRight) {
    double oldDirX = player->camera->dirX;
    player->camera->dirX = player->camera->dirX * cos(-rotateSpeed) - player->camera->dirY * sin(-rotateSpeed);
    player->camera->dirY = oldDirX * sin(-rotateSpeed) + player->camera->dirY * cos(-rotateSpeed);
    double oldPlaneX = player->camera->planeX;
    player->camera->planeX = player->camera->planeX * cos(-rotateSpeed) - player->camera->planeY * sin(-rotateSpeed);
    player->camera->planeY = oldPlaneX * sin(-rotateSpeed) + player->camera->planeY * cos(-rotateSpeed);
  }

  if (inputPacket.get()->rotateLeft) {
    double oldDirX = player->camera->dirX;
    player->camera->dirX = player->camera->dirX * cos(rotateSpeed) - player->camera->dirY * sin(rotateSpeed);
    player->camera->dirY = oldDirX * sin(rotateSpeed) + player->camera->dirY * cos(rotateSpeed);
    double oldPlaneX = player->camera->planeX;
    player->camera->planeX = player->camera->planeX * cos(rotateSpeed) - player->camera->planeY * sin(rotateSpeed);
    player->camera->planeY = oldPlaneX * sin(rotateSpeed) + player->camera->planeY * cos(rotateSpeed);
  }
}
