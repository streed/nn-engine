#include <iostream>

using namespace std;

#include "config.h"

Config::Config(int argc, char **args) {

  if (argc >= 2) {
    cout << "Running game in fullscreen\!" << endl;
    fullscreen = true;
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
}

std::vector<Texture> *Config::getTextures() {
  return &textures;
}

bool Config::getFullscreen() {
  return fullscreen;
}
