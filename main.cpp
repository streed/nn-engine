#include <iostream>
#include <math.h>
#include <SDL2/SDL.h>

using namespace std;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define MAP_WIDTH 24
#define MAP_HEIGHT 24

#define MOVE_SPEED 5
#define ROTATE_SPEED 3

bool KEY_PRESSES[322];
bool quit = false;
bool debug = false;

int worldMap[MAP_WIDTH][MAP_HEIGHT]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

struct World {
  int width;
  int height;
  int *map;

  World(int width, int height, int *map);
  int getMapPoint(int x, int y);
};

World::World(int width, int height, int *map) {
  this->width = width;
  this->height = height;
  this->map = map;
}

int World::getMapPoint(int x, int y) {
  return *(this->map + x * this->height + y);
}

World world = World(MAP_WIDTH,  MAP_HEIGHT, (int *)&worldMap);

struct ColorRGB {
  int r;
  int g;
  int b;

  ColorRGB(int r, int g, int b);
  ColorRGB();
};

ColorRGB::ColorRGB(int r, int g, int b) {
  this->r = r;
  this->g = g;
  this->b = b;
}

ColorRGB::ColorRGB() {
  this->r = 0;
  this->g = 0;
  this->b = 0;
}

ColorRGB operator/(const ColorRGB &color, int div) {
  return ColorRGB(color.r / div, color.g / div, color.b / div);
}

static const ColorRGB RGB_Red(255, 0, 0);
static const ColorRGB RGB_Green(0, 255, 0);
static const ColorRGB RGB_Blue(0, 0, 255);
static const ColorRGB RGB_White(255, 255, 255);
static const ColorRGB RGB_Yellow(255, 255, 0);

ColorRGB getColor(int colorIndex, int side) {
  ColorRGB color;
  switch(colorIndex) {
    case 1: color = RGB_Red; break;
    case 2: color = RGB_Green; break;
    case 3: color = RGB_Blue; break;
    case 4: color = RGB_White; break;
    default: color = RGB_Yellow; break;
  }

  if (side == 1) {
    color = color / 2;
  }

  return color;
}

struct InputPacket {
  bool forward;
  bool backward;
  bool rotateLeft;
  bool rotateRight;
  bool quit;
  
  InputPacket(bool forward, bool backward, bool rotateLeft, bool rotateRight, bool quit);
};

InputPacket::InputPacket( bool f, bool b, bool rl, bool rr, bool q) {
  this->forward = f;
  this->backward = b;
  this->rotateLeft = rl;
  this->rotateRight = rr;
  this->quit = q;
}

InputPacket handleInput () {
  InputPacket packet = InputPacket(KEY_PRESSES[SDLK_w],
                                   KEY_PRESSES[SDLK_s],
                                   KEY_PRESSES[SDLK_a],
                                   KEY_PRESSES[SDLK_d],
                                   KEY_PRESSES[SDLK_q]);
  return packet;
}

ostream& operator << (ostream &os, const InputPacket &ip) {
  return (os << "InputPacket: Forward(" << ip.forward 
            << ") Backward(" << ip.backward << ") RotateLeft(" << ip.rotateLeft
            << ") RotateRight(" << ip.rotateRight << ") Quit(" << ip.quit) << ")";
}

struct Camera {
  double posX;
  double posY;
  double dirX;
  double dirY;
  double planeX;
  double planeY;

  Camera(double posX, double posY, double dirX, double dirY, double planeX, double planeY);
  double getCameraX(int x);
  double getRayDirX(double cameraX);
  double getRayDirY(double cameraX);
};

double Camera::getCameraX(int x) {
  return  2 * x / double(SCREEN_WIDTH) - 1;
}

double Camera::getRayDirX(double cameraX) {
  return this->dirX + this->planeX * cameraX;
}

double Camera::getRayDirY(double cameraX) {
  return this->dirY + this->planeY * cameraX;
}

Camera::Camera(double posX, double posY, double dirX, double dirY, double planeX, double planeY) {
  this->posX = posX;
  this->posY = posY;
  this->dirX = dirX;
  this->dirY = dirY;
  this->planeX = planeX;
  this->planeY = planeY;
}

struct Player {
  Camera *camera;

  Player(Camera *camera);
  int getMapX();
  int getMapY();
  void handleInputs(InputPacket inputPacket, World world, double frameTime);
};

Player::Player(Camera *camera) {
  this->camera = camera;
}


int Player::getMapX() {
  return int(this->camera->posX);
}

int Player::getMapY() {
  return int(this->camera->posY);
}

void Player::handleInputs(InputPacket inputPacket, World world, double frameTime) {
  if (debug) {
    cout << inputPacket << endl;
  }
  double moveSpeed = MOVE_SPEED * frameTime;
  double rotateSpeed = ROTATE_SPEED * frameTime;

  if (inputPacket.forward) {
    double newX = this->camera->posX + this->camera->dirX * moveSpeed;
    double newY = this->camera->posY + this->camera->dirY * moveSpeed;

    if (world.getMapPoint(int(newX), this->camera->posY) == 0) {
      this->camera->posX = newX;
    }

    if (world.getMapPoint(this->camera->posX, int(newY)) == 0) {
      this->camera->posY = newY;
    }
  }

  if (inputPacket.backward) {
    double newX = this->camera->posX - this->camera->dirX * moveSpeed;
    double newY = this->camera->posY - this->camera->dirY * moveSpeed;

    if (world.getMapPoint(int(newX), this->camera->posY) == 0) {
      this->camera->posX = newX;
    }

    if (world.getMapPoint(this->camera->posX, int(newY)) == 0) {
      this->camera->posY = newY;
    }
  }

  if (inputPacket.rotateRight) {
    double oldDirX = this->camera->dirX;
    double newDirX = oldDirX * cos(-rotateSpeed) - this->camera->dirY * sin(-rotateSpeed);
    double newDirY = oldDirX * sin(-rotateSpeed) + this->camera->dirY * cos(-rotateSpeed);
    double oldPlaneX = this->camera->planeX;
    double newPlaneX = oldPlaneX * cos(-rotateSpeed) - this->camera->planeY * sin(-rotateSpeed);
    double newPlaneY = oldPlaneX * sin(-rotateSpeed) + this->camera->planeY * cos(-rotateSpeed);

    this->camera->dirX = newDirX;
    this->camera->dirY = newDirY;
    this->camera->planeX = newPlaneX;
    this->camera->planeY = newPlaneY;
  }

  if (inputPacket.rotateLeft) {
    double oldDirX = this->camera->dirX;
    double newDirX = oldDirX * cos(rotateSpeed) - this->camera->dirY * sin(rotateSpeed);
    double newDirY = oldDirX * sin(rotateSpeed) + this->camera->dirY * cos(rotateSpeed);
    double oldPlaneX = this->camera->planeX;
    double newPlaneX = oldPlaneX * cos(rotateSpeed) - this->camera->planeY * sin(rotateSpeed);
    double newPlaneY = oldPlaneX * sin(rotateSpeed) + this->camera->planeY * cos(rotateSpeed);

    this->camera->dirX = newDirX;
    this->camera->dirY = newDirY;
    this->camera->planeX = newPlaneX;
    this->camera->planeY = newPlaneY;
  }
}

void drawWallSlice(SDL_Renderer *renderer, int x, int bottom, int end, int colorIndex, int side) {
  ColorRGB color = getColor(colorIndex, side);
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLine(renderer, x, bottom, x, end);
}

void clearKeys() {
  for(int i = 0; i < 322; i++) {
    KEY_PRESSES[i] = false;
  }
}

bool handleKeyboard () {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        KEY_PRESSES[event.key.keysym.sym] = true;
        break;
      case SDL_KEYUP:
        KEY_PRESSES[event.key.keysym.sym] = false;
        break;
    }
  }

  return false;
}

int main(int argc, char **args) {
  if (argc > 1) {
    debug = true;
  }
  /*
   * Player data
   */
  Camera camera = Camera(22, 12, -1, 0, 0, 0.66);
  Player player = Player(&camera);

  double time = 0;
  double oldTime = 0;

  SDL_Window *window = NULL;
  SDL_Renderer* renderer = NULL;
  clearKeys();

  if (SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_JOYSTICK) < 0) {
    cout << "SDL Could not initialize! SDL_Error: " << SDL_GetError() << "\n";
  } else {
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_GL_SetSwapInterval(0);

    if (window == NULL) {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
    } else {
      if (debug) {
        cout << "Window created, starting game." << endl;
      }
      SDL_Event e;

      while(!quit) {
        /*
         * Render logic
         */

        oldTime = time;
        time = SDL_GetTicks();
        double frameTime = (time - oldTime) / 1000.0;
        if (debug) {
          cout << "FPS: " << (1 / frameTime) << "\n";
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for(int x = 0; x < SCREEN_WIDTH; x++) {
          double cameraX = player.camera->getCameraX(x);
          double rayDirX = player.camera->getRayDirX(cameraX);
          double rayDirY = player.camera->getRayDirY(cameraX);

          int mapX = player.getMapX();
          int mapY = player.getMapY();;

          double sideDistX;
          double sideDistY;

          double deltaDistX = abs(1 / rayDirX);
          double deltaDistY = abs(1 / rayDirY);

          int stepX;
          int stepY;
          int hit = 0;
          int side;

          if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (camera.posX - mapX) * deltaDistX;
          } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - camera.posX) * deltaDistX;
          }

          if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (camera.posY - mapY) * deltaDistY;
          } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - camera.posY) * deltaDistY;
          }

          while(hit == 0) {
            if (sideDistX < sideDistY) {
              sideDistX += deltaDistX;
              mapX += stepX;
              side = 0;
            } else {
              sideDistY += deltaDistY;
              mapY += stepY;
              side = 1;
            }

            if (world.getMapPoint(mapX, mapY) > 0) {
              hit = 1;
            }
          }

          double perpWallDist;
          if (side == 0) {
            perpWallDist = (mapX - camera.posX + (1 - stepX) / 2) / rayDirX;
          } else {
            perpWallDist = (mapY - camera.posY + (1 - stepY) / 2) / rayDirY;
          }

          int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);
          int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
          if (drawStart < 0) {
            drawStart = 0;
          }

          int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
          if (drawEnd > SCREEN_HEIGHT) {
            drawEnd = SCREEN_HEIGHT - 1;
          }


          drawWallSlice(renderer, x, drawStart, drawEnd, world.getMapPoint(mapX, mapY), side);
        }

        SDL_RenderPresent(renderer);

        /*
         * Input logic
         */
        handleKeyboard();
        InputPacket inputPacket = handleInput();

        player.handleInputs(inputPacket, world, frameTime);

        if (inputPacket.quit) {
          if (debug) {
            cout << "Quitting the game" << endl;
          }
          quit = true;
        }
      }

    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
