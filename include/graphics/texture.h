#ifndef __TEXTURE__
#define __TEXTURE__

#include <SDL2/SDL.h>

#include <fstream>
#include <string>
#include <vector>

#include "color_rgb.h"

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

#endif
