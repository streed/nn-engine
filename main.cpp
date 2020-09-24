#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define NUM_RAYCASTS 32

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

int main() {
  /*
   * Player data
   */
  double posX = 22;
  double posY = 12;
  double dirX = -1;
  double dirY = 0;
  double planeX = 0;
  double planeY = 0.66;

  double time = 0;
  double oldTime = 0;

  SDL_Window *window = NULL;
  SDL_Renderer* renderer = NULL;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    cout << "SDL Could not initialize! SDL_Error: " << SDL_GetError() << "\n";
  } else {
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    if (window == NULL) {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
    } else {
      cout << "A window should show up!\n";
      SDL_Event e;
      bool quit = false;

      while(!quit) {
        /*
         * Input logic
         */
        while(SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) {
            quit = true;
          }

          if (e.type == SDL_KEYDOWN) {
            quit = true;
          }
        }

        /*
         * Render logic
         */

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for(int x = 0; x < NUM_RAYCASTS; x++) {
          double cameraX = 2 * x / double(NUM_RAYCASTS) - 1;
          double rayDirX = dirX + planeX * cameraX;
          double rayDirY = dirY + planeY * cameraX;

          int mapX = int(posX);
          int mapY = int(posY);

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
            sideDistX = (posX - mapX) * deltaDistX;
          } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
          }

          if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
          } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
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

            if (worldMap[mapX][mapY] > 0) {
              hit = 1;
            }
          }

          double perpWallDist;
          if (side == 0) {
            perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
          } else {
            perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;
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

          SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
          SDL_RenderDrawLine(renderer, x * (SCREEN_WIDTH / NUM_RAYCASTS), drawStart, x * (SCREEN_WIDTH / NUM_RAYCASTS), drawEnd);
          SDL_RenderPresent(renderer);
        }
      }

    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
