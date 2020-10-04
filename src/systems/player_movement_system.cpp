#include <cmath>

#include "systems/player_movement_system.h"

#include "engine/engine.h"
#include "coordinator.h"

namespace NN::Systems::BuiltIns {
  void PlayerMovementSystem::update(NN::Engine &engine, double frameTime) {
    NN::Coordinator *coordinator = engine.getCoordinator();

    for (auto const &entity: entities) {
      auto &camera = coordinator->getComponent<Components::Camera>(entity);
      auto &input = coordinator->getComponent<Components::Input>(entity);
      auto &velocity = coordinator->getComponent<Components::Velocity>(entity);

      double moveSpeed = velocity.maxSpeed * frameTime;
      double rotateSpeed = velocity.maxRotateSpeed * frameTime;

      double velocityX = 0;
      double velocityY = 0;

      if (input.forward) {
        velocityX += camera.dirX * moveSpeed;
        velocityY += camera.dirY * moveSpeed;
      }

      if (input.backward) {
        velocityX += -camera.dirX * moveSpeed;
        velocityY += -camera.dirY * moveSpeed;
      }

      if (input.strafeLeft) {
        double perpDirX = camera.dirY;
        double perpDirY = -camera.dirX;

        velocityX += -perpDirX * moveSpeed;
        velocityY += -perpDirY * moveSpeed;
      }

      if (input.strafeRight) {
        double perpDirX = camera.dirY;
        double perpDirY = -camera.dirX;

        velocityX += perpDirX * moveSpeed;
        velocityY += perpDirY * moveSpeed;
      }

      velocity.velocityX = velocityX;
      velocity.velocityY = velocityY;

      if (input.rotateLeft) {
        camera.rotate(rotateSpeed);
      }

      if (input.rotateRight) {
        camera.rotate(-rotateSpeed);
      }

    }

  }
}
