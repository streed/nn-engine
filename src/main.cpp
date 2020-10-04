#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "engine/engine.h"
#include "engine/config.h"
#include "entities.h"
#include "systems/system.h"
#include "systems/physics_system.h"
#include "coordinator.h"
#include "world.h"

using namespace std;

int main(int argc, char **args) {
  NN::Config config(argc, args);
  NN::Engine engine(&config);
  engine.setup();
  engine.run();

  return 0;
}
