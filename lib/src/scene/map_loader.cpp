#include <fstream>
#include <iostream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "scene/map_loader.h"

namespace NN::Scenes {

  MapData MapLoader::loadFromFile(const std::string &filepath) {
    MapData data;

    std::ifstream file(filepath);
    if (!file.is_open()) {
      std::cerr << "Error: Could not open map file: " << filepath << std::endl;
      return data;
    }

    try {
      nlohmann::json json;
      file >> json;

      // Load map grid
      if (json.contains("map")) {
        auto &mapSection = json["map"];
        data.width = mapSection.value("width", 0);
        data.height = mapSection.value("height", 0);

        if (mapSection.contains("tiles")) {
          data.tiles = mapSection["tiles"].get<std::vector<int>>();

          // Validate tile count matches dimensions
          if (static_cast<int>(data.tiles.size()) != data.width * data.height) {
            std::cerr << "Warning: Map tile count (" << data.tiles.size()
                      << ") does not match dimensions ("
                      << data.width << "x" << data.height << ")" << std::endl;
          }
        }
      }

      // Load doors
      if (json.contains("doors")) {
        for (const auto &doorJson : json["doors"]) {
          DoorDef door;
          door.x = doorJson.value("x", 0);
          door.y = doorJson.value("y", 0);
          std::string dir = doorJson.value("direction", "up");
          door.opensUp = (dir == "up");
          door.textureIndex = doorJson.value("texture", 2);
          door.speed = doorJson.value("speed", 2.0);
          door.autoCloseDelay = doorJson.value("autoCloseDelay", 5.0);
          data.doors.push_back(door);
        }
      } else if (json.contains("map") && json["map"].contains("doors")) {
        for (const auto &doorJson : json["map"]["doors"]) {
          DoorDef door;
          door.x = doorJson.value("x", 0);
          door.y = doorJson.value("y", 0);
          std::string dir = doorJson.value("direction", "up");
          door.opensUp = (dir == "up");
          door.textureIndex = doorJson.value("texture", 2);
          door.speed = doorJson.value("speed", 2.0);
          door.autoCloseDelay = doorJson.value("autoCloseDelay", 5.0);
          data.doors.push_back(door);
        }
      }

      // Load entities
      if (json.contains("entities")) {
        for (const auto &entityJson : json["entities"]) {
          EntityDef entity;
          entity.type = entityJson.value("type", "static_sprite");
          entity.name = entityJson.value("name", "");
          entity.posX = entityJson.value("x", 0.0);
          entity.posY = entityJson.value("y", 0.0);

          // Camera properties (for player)
          if (entityJson.contains("camera")) {
            auto &cam = entityJson["camera"];
            entity.dirX = cam.value("dirX", -1.0);
            entity.dirY = cam.value("dirY", 0.0);
            entity.planeX = cam.value("planeX", 0.0);
            entity.planeY = cam.value("planeY", 0.66);
          }

          // Movement properties
          if (entityJson.contains("movement")) {
            auto &mov = entityJson["movement"];
            entity.maxSpeed = mov.value("maxSpeed", 5.0);
            entity.maxRotateSpeed = mov.value("maxRotateSpeed", 3.0);
          }

          // Sprite properties
          if (entityJson.contains("sprite")) {
            auto &spr = entityJson["sprite"];
            entity.textureIndex = spr.value("textureIndex", -1);
            entity.spriteWidth = spr.value("width", 64);
            entity.spriteHeight = spr.value("height", 64);
            entity.spriteScaleX = spr.value("scaleX", 1.0);
            entity.spriteScaleY = spr.value("scaleY", 1.0);
            entity.vMove = spr.value("vMove", 0);
            entity.animated = spr.value("animated", false);
          }

          // AI/follow properties
          if (entityJson.contains("follow")) {
            auto &follow = entityJson["follow"];
            entity.followTarget = follow.value("target", "");
            entity.stopDistance = follow.value("stopDistance", 1.0);
          }

          data.entities.push_back(entity);
        }
      }

    } catch (const nlohmann::json::exception &e) {
      std::cerr << "Error parsing map file: " << e.what() << std::endl;
    }

    return data;
  }

}
