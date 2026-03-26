#include "ui/ui_system.h"
#include "engine/config.h"

#include <algorithm>
#include <cstring>

namespace NN::UI {

  void UISystem::setup(SDL_Renderer *renderer, TTF_Font *font, Config *config) {
    this->font = font;
    this->config = config;
    this->screenWidth = config->getScreenWidth();
    this->screenHeight = config->getScreenHeight();
  }

  void UISystem::addCanvas(UICanvas *canvas) {
    canvases.push_back(canvas);
  }

  void UISystem::removeCanvas(UICanvas *canvas) {
    canvases.erase(
      std::remove(canvases.begin(), canvases.end(), canvas),
      canvases.end());
  }

  void UISystem::render(SDL_Renderer *renderer) {
    // Collect all visible elements from all canvases, sorted by layer
    std::vector<const UIElement*> sortedElements;

    for (const auto *canvas : canvases) {
      for (const auto &element : canvas->getElements()) {
        if (element.visible) {
          sortedElements.push_back(&element);
        }
      }
    }

    std::sort(sortedElements.begin(), sortedElements.end(),
      [](const UIElement *a, const UIElement *b) {
        return a->layer < b->layer;
      });

    for (const auto *element : sortedElements) {
      renderElement(renderer, *element);
    }
  }

  void UISystem::renderElement(SDL_Renderer *renderer, const UIElement &element) {
    switch (element.type) {
      case UIElementType::RECT:
        renderRect(renderer, element);
        break;
      case UIElementType::TEXT:
        renderText(renderer, element);
        break;
      case UIElementType::IMAGE:
        renderImage(renderer, element);
        break;
      case UIElementType::CROSSHAIR:
        renderCrosshair(renderer, element);
        break;
      case UIElementType::BAR:
        renderBar(renderer, element);
        break;
    }
  }

  void UISystem::resolvePosition(const UIElement &element, int &outX, int &outY) const {
    int anchorX = 0, anchorY = 0;

    switch (element.anchor) {
      case Anchor::TOP_LEFT:
      case Anchor::CENTER_LEFT:
      case Anchor::BOTTOM_LEFT:
        anchorX = 0;
        break;
      case Anchor::TOP_CENTER:
      case Anchor::CENTER:
      case Anchor::BOTTOM_CENTER:
        anchorX = screenWidth / 2;
        break;
      case Anchor::TOP_RIGHT:
      case Anchor::CENTER_RIGHT:
      case Anchor::BOTTOM_RIGHT:
        anchorX = screenWidth;
        break;
    }

    switch (element.anchor) {
      case Anchor::TOP_LEFT:
      case Anchor::TOP_CENTER:
      case Anchor::TOP_RIGHT:
        anchorY = 0;
        break;
      case Anchor::CENTER_LEFT:
      case Anchor::CENTER:
      case Anchor::CENTER_RIGHT:
        anchorY = screenHeight / 2;
        break;
      case Anchor::BOTTOM_LEFT:
      case Anchor::BOTTOM_CENTER:
      case Anchor::BOTTOM_RIGHT:
        anchorY = screenHeight;
        break;
    }

    outX = anchorX + element.x;
    outY = anchorY + element.y;
  }

  void UISystem::renderRect(SDL_Renderer *renderer, const UIElement &element) {
    int px, py;
    resolvePosition(element, px, py);

    if (element.filled) {
      SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(renderer, element.fillColor.r, element.fillColor.g,
                             element.fillColor.b, element.fillColor.a);
      SDL_Rect rect = { px, py, element.width, element.height };
      SDL_RenderFillRect(renderer, &rect);
    }

    if (element.borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, element.borderColor.r, element.borderColor.g,
                             element.borderColor.b, element.borderColor.a);
      for (int i = 0; i < element.borderWidth; i++) {
        SDL_Rect borderRect = { px - i, py - i,
                                element.width + 2 * i, element.height + 2 * i };
        SDL_RenderDrawRect(renderer, &borderRect);
      }
    }
  }

  void UISystem::renderText(SDL_Renderer *renderer, const UIElement &element) {
    if (element.text.empty() || !font) return;

    int px, py;
    resolvePosition(element, px, py);

    SDL_Color sdlColor = {
      element.textColor.r,
      element.textColor.g,
      element.textColor.b,
      element.textColor.a
    };

    SDL_Surface *surface = TTF_RenderText_Blended(font, element.text.c_str(), sdlColor);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
      SDL_FreeSurface(surface);
      return;
    }

    // Scale rendered text to match the requested fontSize
    // Font is loaded at a fixed size (32), so scale proportionally
    double scale = (element.fontSize > 0) ? (element.fontSize / 32.0) : 1.0;
    int destW = static_cast<int>(surface->w * scale);
    int destH = static_cast<int>(surface->h * scale);

    SDL_Rect dest = { px, py, destW, destH };
    SDL_RenderCopy(renderer, texture, nullptr, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
  }

  void UISystem::renderImage(SDL_Renderer *renderer, const UIElement &element) {
    if (element.textureIndex < 0 || !config) return;

    int px, py;
    resolvePosition(element, px, py);

    auto *textures = config->getTextures();
    if (element.textureIndex >= static_cast<int>(textures->size())) return;

    auto &tex = textures->at(element.textureIndex);
    auto *pixels = tex.getPixels();
    if (!pixels || pixels->empty()) return;

    // Create SDL texture from pixel data and render it
    // Assume texture is 64x64 (TEXTURE_WIDTH x TEXTURE_HEIGHT)
    int texW = 64, texH = 64;
    SDL_Texture *sdlTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32,
                                            SDL_TEXTUREACCESS_STREAMING, texW, texH);
    if (!sdlTex) return;

    void *texPixels;
    int pitch;
    if (SDL_LockTexture(sdlTex, nullptr, &texPixels, &pitch) == 0) {
      for (int y = 0; y < texH; y++) {
        std::memcpy(static_cast<uint8_t*>(texPixels) + y * pitch,
                    &(*pixels)[y * texW], texW * sizeof(uint32_t));
      }
      SDL_UnlockTexture(sdlTex);
    }

    SDL_SetTextureBlendMode(sdlTex, SDL_BLENDMODE_BLEND);
    SDL_Rect dest = { px, py, element.width, element.height };
    SDL_RenderCopy(renderer, sdlTex, nullptr, &dest);
    SDL_DestroyTexture(sdlTex);
  }

  void UISystem::renderCrosshair(SDL_Renderer *renderer, const UIElement &element) {
    int cx, cy;
    resolvePosition(element, cx, cy);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, element.crosshairColor.r, element.crosshairColor.g,
                           element.crosshairColor.b, element.crosshairColor.a);

    int size = element.crosshairSize;
    int thick = element.crosshairThickness;
    int gap = element.crosshairGap;

    // Horizontal lines (left and right of center gap)
    SDL_Rect left = { cx - size - gap, cy - thick / 2, size, thick };
    SDL_Rect right = { cx + gap, cy - thick / 2, size, thick };
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    // Vertical lines (above and below center gap)
    SDL_Rect top = { cx - thick / 2, cy - size - gap, thick, size };
    SDL_Rect bottom = { cx - thick / 2, cy + gap, thick, size };
    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
  }

  void UISystem::renderBar(SDL_Renderer *renderer, const UIElement &element) {
    int px, py;
    resolvePosition(element, px, py);

    // Draw background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, element.barBackgroundColor.r, element.barBackgroundColor.g,
                           element.barBackgroundColor.b, element.barBackgroundColor.a);
    SDL_Rect bgRect = { px, py, element.width, element.height };
    SDL_RenderFillRect(renderer, &bgRect);

    // Draw fill based on value/maxValue
    double ratio = (element.maxValue > 0.0) ? (element.value / element.maxValue) : 0.0;
    if (ratio < 0.0) ratio = 0.0;
    if (ratio > 1.0) ratio = 1.0;

    int fillWidth = static_cast<int>(element.width * ratio);
    SDL_SetRenderDrawColor(renderer, element.barColor.r, element.barColor.g,
                           element.barColor.b, element.barColor.a);
    SDL_Rect fillRect = { px, py, fillWidth, element.height };
    SDL_RenderFillRect(renderer, &fillRect);

    // Border
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bgRect);
  }

}
