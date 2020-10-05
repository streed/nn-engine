#include <iostream>
using namespace std;
#include <memory>
#include <algorithm>
#include <cmath>

#include "systems/sprite_system.h"
#include "systems/render_system.h"

#include "engine/config.h"
#include "engine/engine.h"
#include "components.h"
#include "coordinator.h"

namespace NN::Systems::BuiltIns {
  void sortSprites(std::shared_ptr<int[]> order, std::shared_ptr<int []> dist, int amount) {
    std::vector<std::pair<double, int> > sprites(amount);
    for(int i = 0; i < amount; i++) {
      sprites[i].first = dist[i];
      sprites[i].second = order[i];
    }

    std::sort(sprites.begin(), sprites.end());

    for(int i = 0; i < amount; i++) {
      dist[i] = sprites[amount - i - 1].first;
      order[i] = sprites[amount - i - 1].second;
    }
  }

  int distanceFromPlayer(Components::Position &player,  Components::Position &sprite) {
    return int((player.posX - sprite.posX) * (player.posX - sprite.posX) +
      (player.posY - sprite.posY) * (player.posY - sprite.posY));
  }

  void SpriteSystem::update(Engine &engine, double frameTime) {
    Coordinator *coordinator = engine.getCoordinator();
    std::vector<Entities::Entity> entitiesVector(entities.begin(), entities.end());

    std::shared_ptr<int []> spriteOrder(new int[entitiesVector.size()]);
    std::shared_ptr<int []> spriteDistance(new int[entitiesVector.size()]);

    auto &playerPosition = coordinator->getComponent<Components::Position>(engine.getCurrentPlayer());
    auto &playerCamera = coordinator->getComponent<Components::Camera>(engine.getCurrentPlayer());

    for (int i = 0; i < entitiesVector.size(); i++) {
      Entities::Entity entity = entitiesVector.at(i);
      auto &spritePosition = coordinator->getComponent<Components::Position>(entity);
      spriteOrder[i] = i;
      spriteDistance[i] = distanceFromPlayer(playerPosition, spritePosition);
    }

    sortSprites(spriteOrder, spriteDistance, entitiesVector.size());

    for (int i = 0; i < entitiesVector.size(); i++) {
      NN::Entities::Entity actualSpriteEntity = entitiesVector.at(spriteOrder[i]);
      auto &sprite = coordinator->getComponent<NN::Components::Sprite>(actualSpriteEntity);
      auto &position = coordinator->getComponent<NN::Components::Position>(actualSpriteEntity);

      double spriteX = position.posX - playerPosition.posX;
      double spriteY = position.posY - playerPosition.posY;

      double invDet = playerCamera.getInvDet();

      double transformX = invDet * (playerCamera.dirY * spriteX - playerCamera.dirX * spriteY);
      double transformY = invDet * (-playerCamera.planeY * spriteX + playerCamera.planeX * spriteY);

      int screenWidth = engine.getConfig()->getScreenWidth();
      int screenHeight = engine.getConfig()->getScreenHeight();

      int spriteScreenX = int((screenWidth / 2) * (1 + transformX / transformY));
      int spriteHeight = abs(int(screenHeight / transformY));

      int drawStartY = -spriteHeight / 2 + screenHeight / 2;
      if (drawStartY < 0) {
        drawStartY = 0;
      }

      int drawEndY = spriteHeight / 2 + screenHeight / 2;

      if (drawEndY >= screenHeight) {
        drawEndY = screenHeight - 1;
      }

      int spriteWidth = abs(int(screenHeight / transformY));
      int drawStartX = -spriteWidth / 2 + spriteScreenX;
      if (drawStartX < 0) {
        drawStartX = 0;
      }

      int drawEndX = spriteWidth / 2 + spriteScreenX;

      if (drawEndX >= screenWidth) {
        drawEndX = screenWidth - 1;
      }

      for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
        int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * sprite.spriteWidth / spriteWidth) / 256;
        if (transformY > 0 && stripe > 0 && stripe < screenWidth && transformY < engine.getRenderSystem()->getZBuffer()[stripe]) {
          for(int y = drawStartY; y < drawEndY; y++) {
            int d = (y) * 256 - screenHeight * 128 + spriteHeight * 128;
            int texY = ((d * sprite.spriteHeight) / spriteHeight) / 256;
            Uint32 color = engine.getConfig()->getTextures()->at(sprite.textureIndex)
              .getPixels()
              ->at(sprite.spriteWidth * texY + texX);

            if ((color & 0x00FFFFFF) != 0) {
              engine.getRenderSystem()->setBufferPixel(stripe, y, color);
            }
          }
        }
      }
    }
  }
}
