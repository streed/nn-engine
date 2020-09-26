#include "config.h"

Config::Config() {
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
}
