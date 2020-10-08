#include <iostream>

using namespace std;

#include <boost/program_options.hpp>
#include <boost/filesystem/directory.hpp>

namespace po = boost::program_options;

#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;

#include "engine/config.h"

#include "graphics/texture.h"

namespace NN {
  Config::Config(int argc, char **args) {
    po::options_description description("Allowed Options");
    description.add_options()
      ("help", "Show this help message")
      ("fullscreen", po::value<bool>(&fullscreen)->default_value(false), "Run game in fullscreen?")
      ("base-game-dir", po::value<std::string>(&baseGameDir)->default_value("./"), "Base directory that contains game data");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, args, description), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << description << endl;
    }

    if (vm.count("fullscreen")) {
      fullscreen = vm["fullscreen"].as<bool>();
    }

    if (vm.count("base-game-dir")) {
      cout << "Loading game data from: " << baseGameDir << endl;
    }

    loadJsonConfig();

    loadTextures();
    loadSprites();
  }

  void Config::loadTextures() {
    textures.push_back(Graphics::Texture(baseGameDir + "textures/bluestone.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/colorstone.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/eagle.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/greystone.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/mossy.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/purplestone.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/redbrick.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/wood.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/barrel.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/greenlight.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/pillar.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/penguin.png"));
    textures.push_back(Graphics::Texture(baseGameDir + "textures/fireball.png"));
  }

  void Config::loadSprites() {
      //Let's just load one sprite...I need to rethink how this is all loaded...
      const std::string path(baseGameDir + "textures\\animated\\1");
      boost::filesystem::directory_iterator end;

      std::vector<int> spriteImagesToLoad;

      for (boost::filesystem::directory_iterator iter(path); iter != end; iter++) {
          spriteImagesToLoad.push_back(std::stoi(iter->path().filename().string()));
      }

      std::sort(spriteImagesToLoad.begin(), spriteImagesToLoad.end());

      for (const auto& spritePath: spriteImagesToLoad) {
          textures.push_back(Graphics::Texture(baseGameDir + "textures/animated/1/" + std::to_string(spritePath)));
      }
  }

  void Config::loadJsonConfig() {
  }

  std::vector<Graphics::Texture> *Config::getTextures() {
    return &textures;
  }

  bool Config::getFullscreen() {
    return fullscreen;
  }

  int Config::getScreenWidth() {
    return 320;
  }

  int Config::getScreenHeight() {
    return 200;
  }
}
