#include <algorithm>
#include <cmath>
#include <vector>

#include "systems/sprite_system.h"
#include "systems/render_system.h"

#include "engine/config.h"
#include "engine/engine.h"
#include "components.h"
#include "coordinator.h"

namespace NN::Systems::BuiltIns {
  static double distanceSquared(const Components::Position &player, const Components::Position &sprite) {
    double dx = player.posX - sprite.posX;
    double dy = player.posY - sprite.posY;
    return dx * dx + dy * dy;
  }

  void SpriteSystem::update(Engine *engine, double frameTime) {
    Coordinator *coordinator = engine->getCoordinator();
    std::vector<Entities::Entity> entitiesVector(entities.begin(), entities.end());
    size_t numSprites = entitiesVector.size();

    auto &playerPosition = coordinator->getComponent<Components::Position>(engine->getCurrentPlayer());
    auto &playerCamera = coordinator->getComponent<Components::Camera>(engine->getCurrentPlayer());

    // Build index+distance pairs and sort by distance (far to near)
    std::vector<std::pair<double, size_t>> spriteDistances(numSprites);
    for (size_t i = 0; i < numSprites; i++) {
      auto &spritePosition = coordinator->getComponent<Components::Position>(entitiesVector[i]);
      spriteDistances[i] = { distanceSquared(playerPosition, spritePosition), i };
    }

    std::sort(spriteDistances.begin(), spriteDistances.end(),
              [](const auto &a, const auto &b) { return a.first > b.first; });

    for (size_t i = 0; i < numSprites; i++) {
      NN::Entities::Entity actualSpriteEntity = entitiesVector[spriteDistances[i].second];
      auto &position = coordinator->getComponent<NN::Components::Position>(actualSpriteEntity);
      auto& spriteAnimation = coordinator->getComponent<NN::Components::AnimatedSprite>(actualSpriteEntity);

      double spriteX = position.posX - playerPosition.posX;
      double spriteY = position.posY - playerPosition.posY;

      double invDet = playerCamera.getInvDet();

      double transformX = invDet * (playerCamera.dirY * spriteX - playerCamera.dirX * spriteY);
      double transformY = invDet * (-playerCamera.planeY * spriteX + playerCamera.planeX * spriteY);
      int vMoveScreen = int(spriteAnimation.vMove / transformY);

      int screenWidth = engine->getConfig()->getScreenWidth();
      int screenHeight = engine->getConfig()->getScreenHeight();

      int spriteScreenX = int((screenWidth / 2) * (1 + transformX / transformY));
      int spriteHeight = abs(int(screenHeight / transformY)) / spriteAnimation.spriteScaleY;

      int drawStartY = -spriteHeight / 2 + screenHeight / 2 + vMoveScreen;
      if (drawStartY < 0) {
        drawStartY = 0;
      }

      int drawEndY = spriteHeight / 2 + screenHeight / 2 + vMoveScreen;

      if (drawEndY >= screenHeight) {
        drawEndY = screenHeight - 1;
      }

      int spriteWidth = abs(int(screenHeight / transformY)) / spriteAnimation.spriteScaleX;
      int drawStartX = -spriteWidth / 2 + spriteScreenX;
      if (drawStartX < 0) {
        drawStartX = 0;
      }

      int drawEndX = spriteWidth / 2 + spriteScreenX;

      if (drawEndX >= screenWidth) {
        drawEndX = screenWidth - 1;
      }

	  int spriteTextureIndex = spriteAnimation.currentFrame;
	  int spriteWidthPixels = spriteAnimation.spriteWidth;
	  int spriteHeightPixels = spriteAnimation.spriteHeight;

      for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
        int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * spriteWidthPixels / spriteWidth) / 256;
        if (transformY > 0 && stripe > 0 && stripe < screenWidth && transformY < engine->getRenderSystem()->getZBuffer()[stripe]) {
          for(int y = drawStartY; y < drawEndY; y++) {
            int d = (y - vMoveScreen) * 256 - screenHeight * 128 + spriteHeight * 128;
            int texY = ((d * spriteHeightPixels) / spriteHeight) / 256;
           Uint32 color = engine->getConfig()->getTextures()->at(spriteTextureIndex)
              .getPixels()
              ->at(spriteWidthPixels * (texY % spriteHeightPixels) + texX);

            if ((color & 0x00FFFFFF) != 0) {
              engine->getRenderSystem()->setBufferPixel(stripe, y, color);
            }
          }
        }
      }
    }
  }
}
