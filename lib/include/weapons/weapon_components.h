#ifndef NN_WEAPON_COMPONENTS_H
#define NN_WEAPON_COMPONENTS_H

#include <string>
#include "entities.h"

namespace NN::Components {

  enum class WeaponType {
    HITSCAN,
    PROJECTILE
  };

  struct Weapon {
    WeaponType type = WeaponType::HITSCAN;

    // Damage
    double damage = 10.0;
    double range = 100.0;           // max range for hitscan / projectile lifetime distance

    // Firing
    double fireRate = 0.5;          // seconds between shots
    double cooldownRemaining = 0.0; // current cooldown timer
    bool firing = false;            // trigger held down

    // Ammo
    int ammo = -1;                  // -1 = infinite
    int maxAmmo = -1;

    // Projectile-specific
    double projectileSpeed = 10.0;
    int projectileTextureIndex = -1;
    double projectileRadius = 0.2;  // collision radius

    // Visual
    int weaponTextureIndex = -1;    // texture for weapon sprite on HUD
    int muzzleFlashTextureIndex = -1;
    double muzzleFlashTimer = 0.0;
    double muzzleFlashDuration = 0.05;

    // Weapon slot
    int slot = 0;
  };

  struct WeaponInventory {
    static constexpr int MAX_WEAPONS = 4;
    int weaponSlots[MAX_WEAPONS] = { -1, -1, -1, -1 }; // entity IDs of weapon entities, -1 = empty
    int activeSlot = 0;
    bool switchRequested = false;
    int switchToSlot = 0;
  };

  struct Projectile {
    double dirX = 0.0;
    double dirY = 0.0;
    double speed = 10.0;
    double damage = 10.0;
    double radius = 0.2;
    double lifetime = 5.0;          // seconds before despawn
    double age = 0.0;
    Entities::Entity owner = 0;     // who fired it
    int textureIndex = -1;
  };

  struct Health {
    double current = 100.0;
    double max = 100.0;

    bool isDead() const { return current <= 0.0; }

    void takeDamage(double amount) {
      current -= amount;
      if (current < 0.0) current = 0.0;
    }

    void heal(double amount) {
      current += amount;
      if (current > max) current = max;
    }
  };

  struct DamageEvent {
    Entities::Entity target;
    Entities::Entity source;
    double damage;
    double hitX, hitY;
  };

}

#endif
