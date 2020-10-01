#ifndef __COMPONENTS_MANAGER__
#define __COMPONENTS_MANAGER__

#include <string>
#include <vector>
#include <unordered_map>

#include "components/component.h"

typedef Component *(*configureComponent)();

class ComponentsManager {
  public:
    static ComponentsManager &get();

    const std::vector<std::string> getComponentNames();
    Component *createComponent(std::string componentName);
    bool registerComponent(const std::string componentName, const configureComponent &generator);

  private:
    ComponentsManager();
    ComponentsManager(const ComponentsManager &);
    ~ComponentsManager();

    std::unordered_map<std::string, configureComponent> generators;
};

#endif
