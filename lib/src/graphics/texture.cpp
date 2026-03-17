#include <iostream>
#include <stdexcept>

#include "graphics/texture.h"
#include "graphics/png.h"

namespace NN::Graphics {

  Texture::Texture(std::string filename): filename(filename) {
    loadImage();
  }

  void Texture::loadFile(std::vector<unsigned char> &buffer) {
    std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

    if (!file.is_open()) {
      std::cerr << "Error: Could not open texture file: " << filename << std::endl;
      return;
    }

    std::streamsize size = 0;

    if (file.seekg(0, std::ios::end).good()) {
      size = file.tellg();
    }

    if (file.seekg(0, std::ios::beg).good()) {
      size -= file.tellg();
    }

    if (size <= 0) {
      std::cerr << "Error: Texture file is empty: " << filename << std::endl;
      return;
    }

    buffer.resize(size_t(size));
    file.read((char *)(&buffer[0]), size);
  }

  void Texture::loadImage() {
    std::vector<unsigned char> file, image;

    loadFile(file);

    if (file.empty()) {
      std::cerr << "Error: No data loaded for texture: " << filename << std::endl;
      return;
    }

    if (decodePNG(image, width, height, file)) {
      std::cerr << "Error: Failed to decode PNG: " << filename << std::endl;
      return;
    }

    colors.resize(image.size() / 4);
    pixels.resize(image.size() / 4);

    for (size_t i = 0; i < colors.size(); i++) {
      colors[i].r = image[i * 4 + 0];
      colors[i].g = image[i * 4 + 1];
      colors[i].b = image[i * 4 + 2];
      pixels[i] = 0x1000000 * image[i * 4 + 3] +
        0x10000 * image[i * 4 + 0] +
        0x100 * image[i * 4 + 1] +
        image[i * 4 + 2];
    }
  }

  std::vector<unsigned int> *Texture::getPixels() {
    return &pixels;
  }

}
