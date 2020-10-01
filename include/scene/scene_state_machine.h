#ifndef __SCENE_STATE_MACHINE__
#define __SCENE_STATE_MACHINE__

#include <memory>
#include <unordered_map>
#include <boost/shared_ptr.hpp>

class Scene;

class SceneStateMachine {
  public:
    SceneStateMachine();

    void processInput();
    void update(double frameTime);
    void lastUpdate(double frameTime);
    void draw();

    int add(boost::shared_ptr<Scene> scene);
    void switchTo(int id);
    void remove(int id);

  private:
    std::unordered_map<int, boost::shared_ptr<Scene>> scenes;
    boost::shared_ptr<Scene> currentScene;
    int insertedSceneId;
};

#endif
