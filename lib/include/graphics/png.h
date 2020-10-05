#ifndef __PNG__
#define __PNG__

#include <vector>

int decodePNG(std::vector<unsigned char>& out_image_32bit,
              unsigned long& image_width,
              unsigned long& image_height,
              const std::vector<unsigned char>& in_png);

int decodePNG(std::vector<unsigned char>& out_image,
              unsigned long& image_width,
              unsigned long& image_height,
              const unsigned char* in_png,
              size_t in_size,
              bool convert_to_rgba32);

#endif
