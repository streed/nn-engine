#include <cmath>
#include <set>
#include <vector>
#include <algorithm>

#include "weapons/weapon_system.h"
#include "engine/engine.h"
#include "engine/config.h"
#include "coordinator.h"
#include "components.h"
#include "raycast.h"
#include "world.h"

namespace NN::Systems::Weapons {

  // Check if a hitscan ray hits a sprite entity using pixel-perfect detection.
  // Uses the same camera transform as the renderer to compute exact texture
  // coordinates where the crosshair ray intersects the sprite, then checks
  // if that pixel is opaque.
  struct SpriteHitResult {
    bool hit = false;
    double distance = 0.0;
    double hitX = 0.0;
    double hitY = 0.0;
  };

  static SpriteHitResult testSpritePixelHit(
      double rayStartX, double rayStartY,
      double rayDirX, double rayDirY,
      const Components::Camera &camera,
      double spritePosX, double spritePosY,
      const Components::AnimatedSprite &sprite,
      double maxDist,
      NN::Config *config)
  {
    SpriteHitResult result;

    // Use the same camera transform the sprite renderer uses
    double spriteX = spritePosX - rayStartX;
    double spriteY = spritePosY - rayStartY;

    double invDet = camera.getInvDet();

    // Transform sprite position to camera space (same as sprite_system.cpp)
    double transformX = invDet * (camera.dirY * spriteX - camera.dirX * spriteY);
    double transformY = invDet * (-camera.planeY * spriteX + camera.planeX * spriteY);

    if (transformY <= 0.0) return result; // Behind camera
    if (transformY > maxDist) return result; // Beyond range

    int screenWidth = config->getScreenWidth();
    int screenHeight = config->getScreenHeight();

    // Screen X position of sprite center
    int spriteScreenX = static_cast<int>((screenWidth / 2) * (1.0 + transformX / transformY));

    // Sprite dimensions on screen (matching renderer)
    int spriteHeightOnScreen = std::abs(static_cast<int>(screenHeight / transformY)) / sprite.spriteScaleY;
    int spriteWidthOnScreen = std::abs(static_cast<int>(screenHeight / transformY)) / sprite.spriteScaleX;

    if (spriteWidthOnScreen <= 0 || spriteHeightOnScreen <= 0) return result;

    // Compute draw bounds (same as renderer)
    int vMoveScreen = static_cast<int>(sprite.vMove / transformY);

    int drawStartX = -spriteWidthOnScreen / 2 + spriteScreenX;
    int drawEndX = spriteWidthOnScreen / 2 + spriteScreenX;
    int drawStartY = -spriteHeightOnScreen / 2 + screenHeight / 2 + vMoveScreen;
    int drawEndY = spriteHeightOnScreen / 2 + screenHeight / 2 + vMoveScreen;

    // The crosshair is at screen center
    int crosshairX = screenWidth / 2;
    int crosshairY = screenHeight / 2;

    // Check if crosshair falls within the sprite's screen bounds
    if (crosshairX < drawStartX || crosshairX >= drawEndX) return result;
    if (crosshairY < drawStartY || crosshairY >= drawEndY) return result;

    // Compute texture coordinates at the crosshair position
    // (same math as sprite_system.cpp)
    int texX = static_cast<int>(
      256 * (crosshairX - (-spriteWidthOnScreen / 2 + spriteScreenX))
      * sprite.spriteWidth / spriteWidthOnScreen
    ) / 256;

    int d = (crosshairY - vMoveScreen) * 256 - screenHeight * 128 + spriteHeightOnScreen * 128;
    int texY = ((d * sprite.spriteHeight) / spriteHeightOnScreen) / 256;

    // Clamp texture coordinates
    if (texX < 0) texX = 0;
    if (texX >= sprite.spriteWidth) texX = sprite.spriteWidth - 1;
    if (texY < 0) texY = 0;
    if (texY >= sprite.spriteHeight) texY = sprite.spriteHeight - 1;

    // Sample the texture pixel
    int textureIndex = sprite.currentFrame;
    auto *textures = config->getTextures();
    if (textureIndex < 0 || textureIndex >= static_cast<int>(textures->size())) {
      return result;
    }

    auto *pixels = textures->at(textureIndex).getPixels();
    if (!pixels || pixels->empty()) return result;

    int pixelIndex = sprite.spriteWidth * texY + texX;
    if (pixelIndex < 0 || pixelIndex >= static_cast<int>(pixels->size())) {
      return result;
    }

    uint32_t color = pixels->at(pixelIndex);

    // Pixel-perfect: only count as hit if the exact pixel is opaque
    if ((color & 0x00FFFFFF) == 0) {
      return result; // Transparent pixel — miss
    }

    // Hit confirmed
    result.hit = true;
    result.distance = transformY; // perpendicular distance (same as zBuffer)
    result.hitX = rayStartX + rayDirX * transformY;
    result.hitY = rayStartY + rayDirY * transformY;
    return result;
  }

  void WeaponSystem::update(NN::Engine *engine, double frameTime) {
    damageEvents.clear();

    Coordinator *coordinator = engine->getCoordinator();

    for (auto const &entity : entities) {
      auto &weapon = coordinator->getComponent<Components::Weapon>(entity);

      // Update cooldown
      if (weapon.cooldownRemaining > 0.0) {
        weapon.cooldownRemaining -= frameTime;
        if (weapon.cooldownRemaining < 0.0) {
          weapon.cooldownRemaining = 0.0;
        }
      }

      // Update muzzle flash
      if (weapon.muzzleFlashTimer > 0.0) {
        weapon.muzzleFlashTimer -= frameTime;
        if (weapon.muzzleFlashTimer < 0.0) {
          weapon.muzzleFlashTimer = 0.0;
        }
      }

      // Check if we should fire
      if (weapon.firing && weapon.cooldownRemaining <= 0.0) {
        if (weapon.ammo != 0) {
          if (weapon.ammo > 0) {
            weapon.ammo--;
          }

          weapon.cooldownRemaining = weapon.fireRate;
          weapon.muzzleFlashTimer = weapon.muzzleFlashDuration;

          switch (weapon.type) {
            case Components::WeaponType::HITSCAN:
              handleHitscan(engine, entity, frameTime);
              break;
            case Components::WeaponType::PROJECTILE:
              handleProjectileFire(engine, entity, frameTime);
              break;
          }
        }
      }
    }
  }

  void WeaponSystem::handleHitscan(Engine *engine, Entities::Entity weaponEntity, double frameTime) {
    Coordinator *coordinator = engine->getCoordinator();
    auto &weapon = coordinator->getComponent<Components::Weapon>(weaponEntity);

    Entities::Entity player = engine->getCurrentPlayer();
    auto &playerPos = coordinator->getComponent<Components::Position>(player);
    auto &playerCam = coordinator->getComponent<Components::Camera>(player);

    // First, find wall distance along the center ray
    Utils::RayCast ray(playerPos.posX, playerPos.posY, playerCam.dirX, playerCam.dirY);
    Utils::RayCastHit wallHit = ray.collideWorld(engine->getWorld());

    double maxDist = std::min(wallHit.perpWallDist, weapon.range);

    // Now test against every entity that has Position + AnimatedSprite + Health.
    // We iterate the spriteSystem's entities since those are the visible ones.
    // The weapon system's own entity set only contains weapon holders,
    // so we need to check a broader set. We use the sprite system's entities
    // via the render system's z-buffer approach — but simpler: just iterate
    // all entities that have the right components.
    //
    // Since we can't easily query cross-system, we iterate our hittableEntities
    // set which is populated by the engine during registration.

    double closestDist = maxDist;
    Entities::Entity closestTarget = 0;
    bool hitFound = false;
    double hitX = 0, hitY = 0;

    for (auto const &targetEntity : hittableEntities) {
      if (targetEntity == player) continue;

      auto &targetPos = coordinator->getComponent<Components::Position>(targetEntity);
      auto &targetSprite = coordinator->getComponent<Components::AnimatedSprite>(targetEntity);

      SpriteHitResult spriteHit = testSpritePixelHit(
        playerPos.posX, playerPos.posY,
        playerCam.dirX, playerCam.dirY,
        playerCam,
        targetPos.posX, targetPos.posY,
        targetSprite,
        closestDist,
        engine->getConfig()
      );

      if (spriteHit.hit && spriteHit.distance < closestDist) {
        closestDist = spriteHit.distance;
        closestTarget = targetEntity;
        hitX = spriteHit.hitX;
        hitY = spriteHit.hitY;
        hitFound = true;
      }
    }

    if (hitFound) {
      Components::DamageEvent event;
      event.target = closestTarget;
      event.source = player;
      event.damage = weapon.damage;
      event.hitX = hitX;
      event.hitY = hitY;
      damageEvents.push_back(event);
    }
  }

  void WeaponSystem::handleProjectileFire(Engine *engine, Entities::Entity weaponEntity, double frameTime) {
    Coordinator *coordinator = engine->getCoordinator();
    auto &weapon = coordinator->getComponent<Components::Weapon>(weaponEntity);

    Entities::Entity player = engine->getCurrentPlayer();
    auto &playerPos = coordinator->getComponent<Components::Position>(player);
    auto &playerCam = coordinator->getComponent<Components::Camera>(player);

    // Create a new projectile entity
    Entities::Entity projectile = coordinator->createEntity();

    double spawnX = playerPos.posX + playerCam.dirX * 0.5;
    double spawnY = playerPos.posY + playerCam.dirY * 0.5;

    coordinator->addComponent<Components::Position>(projectile,
      Components::Position{ spawnX, spawnY });

    coordinator->addComponent<Components::Velocity>(projectile,
      Components::Velocity{
        playerCam.dirX * weapon.projectileSpeed,
        playerCam.dirY * weapon.projectileSpeed,
        weapon.projectileSpeed,
        0.0
      });

    coordinator->addComponent<Components::Projectile>(projectile,
      Components::Projectile{
        playerCam.dirX,
        playerCam.dirY,
        weapon.projectileSpeed,
        weapon.damage,
        weapon.projectileRadius,
        weapon.range / weapon.projectileSpeed,
        0.0,
        player,
        weapon.projectileTextureIndex
      });

    if (weapon.projectileTextureIndex >= 0) {
      coordinator->addComponent<Components::AnimatedSprite>(projectile,
        Components::AnimatedSprite{
          weapon.projectileTextureIndex,
          weapon.projectileTextureIndex,
          weapon.projectileTextureIndex,
          0,
          64, 64,
          4.0, 4.0,
          0,
          true,
          {}
        });
    }
  }

}
