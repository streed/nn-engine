#ifndef NN_UI_SYSTEM_H
#define NN_UI_SYSTEM_H

#include <vector>
#include <algorithm>

#if defined __APPLE__ || _MSC_VER
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#include "ui/ui_components.h"

namespace NN {
  class Config;

  namespace UI {

    class UISystem {
      public:
        UISystem() = default;

        void setup(SDL_Renderer *renderer, TTF_Font *font, Config *config);

        // Renders all UI elements from all registered canvases
        void render(SDL_Renderer *renderer);

        // Register/unregister canvases
        void addCanvas(UICanvas *canvas);
        void removeCanvas(UICanvas *canvas);

      private:
        void renderElement(SDL_Renderer *renderer, const UIElement &element);
        void renderRect(SDL_Renderer *renderer, const UIElement &element);
        void renderText(SDL_Renderer *renderer, const UIElement &element);
        void renderImage(SDL_Renderer *renderer, const UIElement &element);
        void renderCrosshair(SDL_Renderer *renderer, const UIElement &element);
        void renderBar(SDL_Renderer *renderer, const UIElement &element);

        // Resolve anchor-relative position to absolute screen position
        void resolvePosition(const UIElement &element, int &outX, int &outY) const;

        std::vector<UICanvas*> canvases;
        TTF_Font *font = nullptr;
        Config *config = nullptr;
        int screenWidth = 0;
        int screenHeight = 0;
    };

  }
}

#endif
