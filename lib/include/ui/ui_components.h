#ifndef NN_UI_COMPONENTS_H
#define NN_UI_COMPONENTS_H

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace NN::UI {

  enum class Anchor {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
  };

  struct Color {
    uint8_t r, g, b, a;

    Color() : r(255), g(255), b(255), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
      : r(r), g(g), b(b), a(a) {}

    uint32_t toUint32() const {
      return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | b;
    }
  };

  enum class UIElementType {
    RECT,
    TEXT,
    IMAGE,
    CROSSHAIR,
    BAR
  };

  struct UIElement {
    UIElementType type = UIElementType::RECT;
    bool visible = true;

    // Position (pixels from anchor point)
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    Anchor anchor = Anchor::TOP_LEFT;

    // Rect properties
    Color fillColor;
    Color borderColor;
    int borderWidth = 0;
    bool filled = true;

    // Text properties
    std::string text;
    Color textColor;
    int fontSize = 16;

    // Image properties (texture index in config textures)
    int textureIndex = -1;

    // Bar properties (for health bars, ammo bars, etc.)
    double value = 1.0;      // 0.0 to 1.0
    double maxValue = 1.0;
    Color barColor;
    Color barBackgroundColor;

    // Crosshair properties
    int crosshairSize = 10;
    int crosshairThickness = 2;
    int crosshairGap = 4;
    Color crosshairColor;

    // Layer ordering (higher = rendered on top)
    int layer = 0;
  };

  class UICanvas {
    public:
      UICanvas() = default;

      int addElement(const UIElement &element) {
        int id = static_cast<int>(elements.size());
        elements.push_back(element);
        return id;
      }

      void removeElement(int id) {
        if (id >= 0 && id < static_cast<int>(elements.size())) {
          elements[id].visible = false;
          // Mark as removed
          removed.push_back(id);
        }
      }

      UIElement &getElement(int id) {
        return elements[id];
      }

      const UIElement &getElement(int id) const {
        return elements[id];
      }

      const std::vector<UIElement> &getElements() const {
        return elements;
      }

      void clear() {
        elements.clear();
        removed.clear();
      }

      // Convenience factory methods
      int addRect(int x, int y, int w, int h, Color color,
                  Anchor anchor = Anchor::TOP_LEFT, int layer = 0) {
        UIElement el;
        el.type = UIElementType::RECT;
        el.x = x; el.y = y; el.width = w; el.height = h;
        el.fillColor = color;
        el.anchor = anchor;
        el.layer = layer;
        return addElement(el);
      }

      int addText(int x, int y, const std::string &text, Color color,
                  int fontSize = 16, Anchor anchor = Anchor::TOP_LEFT, int layer = 10) {
        UIElement el;
        el.type = UIElementType::TEXT;
        el.x = x; el.y = y;
        el.text = text;
        el.textColor = color;
        el.fontSize = fontSize;
        el.anchor = anchor;
        el.layer = layer;
        return addElement(el);
      }

      int addImage(int x, int y, int w, int h, int textureIndex,
                   Anchor anchor = Anchor::TOP_LEFT, int layer = 0) {
        UIElement el;
        el.type = UIElementType::IMAGE;
        el.x = x; el.y = y; el.width = w; el.height = h;
        el.textureIndex = textureIndex;
        el.anchor = anchor;
        el.layer = layer;
        return addElement(el);
      }

      int addCrosshair(Color color, int size = 10, int thickness = 2,
                       int gap = 4, int layer = 20) {
        UIElement el;
        el.type = UIElementType::CROSSHAIR;
        el.anchor = Anchor::CENTER;
        el.crosshairColor = color;
        el.crosshairSize = size;
        el.crosshairThickness = thickness;
        el.crosshairGap = gap;
        el.layer = layer;
        return addElement(el);
      }

      int addBar(int x, int y, int w, int h, double value, double maxValue,
                 Color barColor, Color bgColor,
                 Anchor anchor = Anchor::TOP_LEFT, int layer = 5) {
        UIElement el;
        el.type = UIElementType::BAR;
        el.x = x; el.y = y; el.width = w; el.height = h;
        el.value = value;
        el.maxValue = maxValue;
        el.barColor = barColor;
        el.barBackgroundColor = bgColor;
        el.anchor = anchor;
        el.layer = layer;
        return addElement(el);
      }

    private:
      std::vector<UIElement> elements;
      std::vector<int> removed;
  };

}

#endif
