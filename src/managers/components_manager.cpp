#include <boost/assert.hpp>

#include "managers/components_manager.h"

#include "components/component.h"

ComponentsManager::ComponentsManager() {}

ComponentsManager::ComponentsManager(const ComponentsManager &componentsManager) {
  generators = componentsManager.generators;
}

ComponentsManager::~ComponentsManager() {}

ComponentsManager &ComponentsManager::get() {
  static ComponentsManager instance;
  return instance;
}

const std::vector<std::string> ComponentsManager::getComponentNames() {
  std::vector<std::string> names;

  for (auto const &pair: generators) {
    names.push_back(pair.first);
  }

  return names;
}

bool ComponentsManager::registerComponent(const std::string componentName, const configureComponent &generator) {
  return generators.insert(std::make_pair(componentName, generator)).second;
}

Component *ComponentsManager::createComponent(std::string componentName) {
  auto it = generators.find(componentName);

  BOOST_ASSERT_MSG(it != generators.end(), "ComponentsManager: Could not find that Component by name");

  return it->second();
}
