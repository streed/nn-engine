#include "entity_factory.h"

EntityFactory::EntityFactory() {}

EntityFactory::EntityFactory(const EntityFactory &entityFactory) {
  generators = entityFactory.generators;
}

EntityFactory & EntityFactory::get() {
  static EntityFactory instance;
  return instance;
}

EntityFactory::~EntityFactory() {}

const std::vector<std::string> EntityFactory::getEntityNames() {
  std::vector<std::string> names;
  for (auto const &pair: generators) {
    names.push_back(pair.first);
  }

  return names;
}

const Entity *EntityFactory::createEntity(std::string entityName) {
  auto it = generators.find(entityName);

  if (it != generators.end()) {
    return it->second();
  } else {
    return NULL;
  }
}

bool EntityFactory::registerEntity(const std::string entityName, const entityGenerator &generator) {
  return generators.insert(std::make_pair(entityName, generator)).second;
}
