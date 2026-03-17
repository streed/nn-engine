#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

namespace po = boost::program_options;

#include <nlohmann/json.hpp>

#include "engine/config.h"
#include "graphics/texture.h"

namespace NN {
  Config::Config(int argc, char **args)
    : fullscreen(false), screenWidth(320), screenHeight(200) {

    po::options_description description("Allowed Options");
    description.add_options()
      ("help", "Show this help message")
      ("fullscreen", po::value<bool>(&fullscreen)->default_value(false), "Run game in fullscreen?")
      ("base-game-dir", po::value<std::string>(&baseGameDir)->default_value("./"), "Base directory that contains game data");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, args, description), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << description << std::endl;
    }

    if (vm.count("fullscreen")) {
      fullscreen = vm["fullscreen"].as<bool>();
    }

    loadJsonConfig();
    loadTextures();
  }

  void Config::loadJsonConfig() {
    std::string configPath = baseGameDir + "game.json";
    std::ifstream configFile(configPath);

    if (!configFile.is_open()) {
      std::cerr << "Warning: Could not open config file: " << configPath
                << ", using defaults" << std::endl;
      return;
    }

    try {
      nlohmann::json config;
      configFile >> config;

      if (config.contains("settings")) {
        auto &settings = config["settings"];

        if (settings.contains("fullscreen")) {
          fullscreen = settings["fullscreen"].get<bool>();
        }

        if (settings.contains("screen")) {
          auto &screen = settings["screen"];
          if (screen.contains("width")) {
            screenWidth = screen["width"].get<int>();
          }
          if (screen.contains("height")) {
            screenHeight = screen["height"].get<int>();
          }
        }
      }

      if (config.contains("textures")) {
        textureFiles.clear();
        for (const auto &tex : config["textures"]) {
          textureFiles.push_back(tex.get<std::string>());
        }
      }
    } catch (const nlohmann::json::exception &e) {
      std::cerr << "Error parsing config file: " << e.what() << std::endl;
    }
  }

  void Config::loadTextures() {
    if (!textureFiles.empty()) {
      for (const auto &texFile : textureFiles) {
        std::string fullPath = baseGameDir + texFile;
        textures.push_back(Graphics::Texture(fullPath));
      }
    } else {
      // Fallback: load default textures
      const char *defaultTextures[] = {
        "textures/bluestone.png",
        "textures/colorstone.png",
        "textures/eagle.png",
        "textures/greystone.png",
        "textures/mossy.png",
        "textures/purplestone.png",
        "textures/redbrick.png",
        "textures/wood.png",
        "textures/barrel.png",
        "textures/greenlight.png",
        "textures/pillar.png",
        "textures/penguin.png",
        "textures/fireball.png",
      };
      for (const auto &tex : defaultTextures) {
        textures.push_back(Graphics::Texture(baseGameDir + tex));
      }
    }
  }

  std::vector<Graphics::Texture> *Config::getTextures() {
    return &textures;
  }

  bool Config::getFullscreen() {
    return fullscreen;
  }

  int Config::getScreenWidth() {
    return screenWidth;
  }

  int Config::getScreenHeight() {
    return screenHeight;
  }

  const std::string &Config::getBaseGameDir() const {
    return baseGameDir;
  }
}
