#ifndef NN_CONFIG_H
#define NN_CONFIG_H

#include <string>
#include <vector>

#include "graphics/texture.h"

namespace NN {
  class Config {
    std::vector<Graphics::Texture> textures;
    bool fullscreen;
    std::string baseGameDir;
    int screenWidth;
    int screenHeight;
    std::vector<std::string> textureFiles;

    public:
    Config(int argc, char **args);

    std::vector<Graphics::Texture> *getTextures();
    bool getFullscreen();

    int getScreenWidth();
    int getScreenHeight();

    const std::string &getBaseGameDir() const;

    private:
    void loadJsonConfig();
    void loadTextures();
  };
}
#endif
