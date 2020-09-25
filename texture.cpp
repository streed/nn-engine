#include "texture.h"
#include "png.h"

Texture::Texture(int width, int height, std::string filename): width(width),
                                                               height(height),
                                                               filename(filename) {
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

  pixels.resize(image.size() / 4);

  for (size_t i = 0; i < pixels.size(); i++) {
    pixels[i].r = image[i * 4 + 0];
    pixels[i].g = image[i * 4 + 0];
    pixels[i].b = image[i * 4 + 0];
  }
}
