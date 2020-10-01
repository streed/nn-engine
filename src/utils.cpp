#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "utils.h"

// Taken from Quake3 source code.
float inverseSqrt(float number) {
  const float threeHalfs = 1.5F;

  float x2 = number * 0.5F;
  float y = number;

  // Use specific type to ensure that it is 32bits long, 64bits messes this up.
  Uint32 i = *(Uint32 *)&y;

  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (threeHalfs - (x2 * y * y));
  return y;
}
