#ifndef __GRAPHICS_COMPONENT__
#define __GRAPHICS_COMPONENT__

class Engine;
class GameObject;

class GraphicsComponent {
  public:
    virtual GraphicsComponent(const GraphicsComponent &) {}
    virtual ~GraphicsComponent() {}
    virtual void update(GameObject &object, Engine &engine) = 0;
}
