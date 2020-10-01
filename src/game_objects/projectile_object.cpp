#include "game_objects/projectile_object.h"

#include "components/physics_component.h"
#include "components/graphics_component.h"

ProjectileObject::~ProjectileObject() {
  delete physicsComponent;
  delete graphicsComponent;
}

void ProjectileObject::update(Engine &engine, World &world, double frameTime) {
  physicsComponent->update(this, engine, world, frameTime);
}

void ProjectileObject::draw(Engine &engine) {
  graphicsComponent->update(this, engine);
}
