#ifndef __ENTITY_FACTORY__
#define __ENTITY_FACTORY__

#include <string>
#include <vector>
#include <unordered_map>

class Entity;

typedef Entity *(*entityGenerator)();

class EntityFactory {
  public:
    static EntityFactory &get();
    const std::vector<std::string> getEntityNames();
    const Entity *createEntity(std::string entityName);
    bool registerEntity(const std::string entityName, const entityGenerator& generator);

  private:
    EntityFactory();
    EntityFactory(const EntityFactory &);
    ~EntityFactory();

    std::unordered_map<std::string, entityGenerator> generators;
};

#endif
