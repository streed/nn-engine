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

  // Check if a hitscan ray hits a sprite entity by testing the actual sprite pixels.
  // Projects the ray into sprite-space and samples the texture to see if
  // a non-transparent pixel is hit.
  struct SpriteHitResult {
    bool hit = false;
    double distance = 0.0;
    double hitX = 0.0;
    double hitY = 0.0;
  };

  static SpriteHitResult testSpritePixelHit(
      double rayStartX, double rayStartY,
      double rayDirX, double rayDirY,
      double spritePosX, double spritePosY,
      const Components::AnimatedSprite &sprite,
      double maxDist,
      NN::Config *config)
  {
    SpriteHitResult result;

    // Vector from ray origin to sprite center
    double toSpriteX = spritePosX - rayStartX;
    double toSpriteY = spritePosY - rayStartY;

    // Distance along ray to the sprite's perpendicular plane
    // Using the same projection the renderer uses:
    // We project the sprite position onto the ray direction
    double dot = toSpriteX * rayDirX + toSpriteY * rayDirY;
    if (dot <= 0.0) return result; // Sprite is behind us
    if (dot > maxDist) return result; // Beyond weapon range or wall

    // Perpendicular distance from ray to sprite center
    // Cross product gives signed perpendicular distance
    double cross = toSpriteX * rayDirY - toSpriteY * rayDirX;
    // |cross| / |rayDir| = perpendicular distance, but rayDir should be unit-length
    double rayLen = std::sqrt(rayDirX * rayDirX + rayDirY * rayDirY);
    double perpDist = cross / rayLen;

    // The sprite has a world-space width based on its distance.
    // In the renderer, spriteHeight = screenHeight / transformY, and
    // spriteWidth = screenHeight / transformY / scaleX.
    // In world space, the sprite occupies roughly 1/scaleX units wide at its position.
    double worldSpriteHalfWidth = 0.5 / sprite.spriteScaleX;

    // Check if the ray passes within the sprite's width
    if (std::fabs(perpDist) > worldSpriteHalfWidth) return result;

    // Now determine the texture X coordinate hit by the ray
    // perpDist ranges from -worldSpriteHalfWidth to +worldSpriteHalfWidth
    // Map to 0..spriteWidth pixels
    double normalizedX = (perpDist + worldSpriteHalfWidth) / (2.0 * worldSpriteHalfWidth);
    int texX = static_cast<int>(normalizedX * sprite.spriteWidth);
    if (texX < 0) texX = 0;
    if (texX >= sprite.spriteWidth) texX = sprite.spriteWidth - 1;

    // For the Y axis in this 2.5D engine, we always shoot from the center of the screen
    // (eye level), so we test the middle row of the sprite texture.
    // A more advanced version could account for vertical aim offset.
    int texY = sprite.spriteHeight / 2;

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

    // Check if pixel is non-transparent (same check the sprite renderer uses)
    if ((color & 0x00FFFFFF) == 0) {
      // Transparent pixel — scan nearby rows to be more forgiving
      // Check a vertical band of pixels around center
      bool foundSolid = false;
      int scanRange = sprite.spriteHeight / 4;
      for (int dy = -scanRange; dy <= scanRange && !foundSolid; dy++) {
        int scanY = texY + dy;
        if (scanY < 0 || scanY >= sprite.spriteHeight) continue;
        int scanIdx = sprite.spriteWidth * scanY + texX;
        if (scanIdx >= 0 && scanIdx < static_cast<int>(pixels->size())) {
          uint32_t scanColor = pixels->at(scanIdx);
          if ((scanColor & 0x00FFFFFF) != 0) {
            foundSolid = true;
          }
        }
      }
      if (!foundSolid) return result;
    }

    // Hit confirmed
    result.hit = true;
    result.distance = dot;
    result.hitX = rayStartX + rayDirX * dot;
    result.hitY = rayStartY + rayDirY * dot;
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
