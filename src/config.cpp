#include <iostream>

using namespace std;

#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include "config.h"

#include "graphics/texture.h"

Config::Config(int argc, char **args) {
  po::options_description description("Allowed Options");
  description.add_options()
    ("help", "Show this help message")
    ("fullscreen", po::value<bool>(&fullscreen)->default_value(false), "Run game in fullscreen?");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, args, description), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << description << endl;
  }

  if (vm.count("fullscreen")) {
    fullscreen = vm["fullscreen"].as<bool>();
  }

  loadTextures();
}

void Config::loadTextures() {
  textures.push_back(Texture("./textures/bluestone.png"));
  textures.push_back(Texture("./textures/colorstone.png"));
  textures.push_back(Texture("./textures/eagle.png"));
  textures.push_back(Texture("./textures/greystone.png"));
  textures.push_back(Texture("./textures/mossy.png"));
  textures.push_back(Texture("./textures/purplestone.png"));
  textures.push_back(Texture("./textures/redbrick.png"));
  textures.push_back(Texture("./textures/wood.png"));
  textures.push_back(Texture("./textures/barrel.png"));
  textures.push_back(Texture("./textures/greenlight.png"));
  textures.push_back(Texture("./textures/pillar.png"));
  textures.push_back(Texture("./textures/penguin.png"));
  textures.push_back(Texture("./textures/fireball.png"));
}

std::vector<Texture> *Config::getTextures() {
  return &textures;
}

bool Config::getFullscreen() {
  return fullscreen;
}
