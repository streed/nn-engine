#include <iostream>

#include "texture.h"
#include "png.h"

Texture::Texture(std::string filename): filename(filename) {
  std::cout << "Loading texture: " << filename << std::endl;
  loadImage();
}

void Texture::loadFile(std::vector<unsigned char> &buffer) {
  std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

  std::streamsize size = 0;

  if (file.seekg(0, std::ios::end).good()) {
    size = file.tellg();
  }

  if (file.seekg(0, std::ios::beg).good()) {
    size -= file.tellg();
  }

  buffer.resize(size_t(size));
  if (size > 0) {
    file.read((char *)(&buffer[0]), size);
  }
}

void Texture::loadImage() {
  std::vector<unsigned char> file, image;

  loadFile(file);
  
  if (decodePNG(image, width, height, file)) {
    return;
  }

  colors.resize(image.size() / 4);
  pixels.resize(image.size() / 4);

  for (size_t i = 0; i < colors.size(); i++) {
    colors[i].r = image[i * 4 + 0];
    colors[i].g = image[i * 4 + 0];
    colors[i].b = image[i * 4 + 0];
    pixels[i] = 0x1000000 * image[i * 4 + 3] +
                0x10000 * image[i * 4 + 0] +
                0x100 * image[i * 4 + 1] +
                image[i * 4 + 2];
  }
}

std::vector<unsigned int> *Texture::getPixels() {
  return &pixels;
}
