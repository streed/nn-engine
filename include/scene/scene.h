#ifndef __SCENE__
#define __SCENE__

class Scene {
  public:
    virtual void onCreate() = 0;
    virtual void onDestroy() = 0;
    virtual void onActivate() {};
    virtual void onDeactivate() {};
    virtual void processInput() {};
    virtual void update(double frameTime) {};
    virtual void lastUpdate(double frameTime) {};
    virtual void draw() {};
};

#endif
