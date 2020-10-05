#ifndef __TEXTURE__
#define __TEXTURE__

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include <fstream>
#include <string>
#include <vector>

#include "color_rgb.h"

namespace NN::Graphics {
  class Texture {
    unsigned long width;
    unsigned long height;

    std::string filename;
    std::vector<ColorRGB> colors;
    std::vector<Uint32> pixels;

    void loadFile(std::vector<unsigned char> &buffer);
    void loadImage();

    public:
    Texture(std::string filename);
    std::vector<unsigned int> *getPixels();
  };
}
#endif
