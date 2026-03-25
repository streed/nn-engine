#ifndef NN_MAP_LOADER_H
#define NN_MAP_LOADER_H

#include <string>
#include <vector>

class World;

namespace NN {
  class Engine;

  namespace Scenes {

    struct EntityDef {
      std::string type;  // "player", "npc", "static_sprite"
      std::string name;  // unique name for entity referencing (e.g., follow targets)
      double posX;
      double posY;

      // Camera (for player)
      double dirX = -1;
      double dirY = 0;
      double planeX = 0;
      double planeY = 0.66;

      // Velocity
      double maxSpeed = 5;
      double maxRotateSpeed = 3;

      // Sprite
      int textureIndex = -1;
      int spriteWidth = 64;
      int spriteHeight = 64;
      double spriteScaleX = 1;
      double spriteScaleY = 1;
      int vMove = 0;
      bool animated = false;

      // NPC follow target
      std::string followTarget;
      double stopDistance = 1.0;

      // Health
      double health = 0;
      double maxHealth = 0;
    };

    struct DoorDef {
      int x;
      int y;
      bool opensUp = true;       // "up" or "down"
      int textureIndex = 2;      // default: eagle texture
      double speed = 2.0;
      double autoCloseDelay = 5.0;
    };

    struct MapData {
      int width = 0;
      int height = 0;
      std::vector<int> tiles;
      std::vector<EntityDef> entities;
      std::vector<DoorDef> doors;
    };

    class MapLoader {
      public:
        static MapData loadFromFile(const std::string &filepath);
    };

  }
}
#endif
