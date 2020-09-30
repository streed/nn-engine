#include "game_objects/imp.h"

#include "game_objects/player.h"

#include "components/imp_ai_component.h"
#include "engine.h"
#include "world.h"

void Imp::update(Engine &engine, World &world, double frameTime) {
  impAIComponent.update(*this, *engine.getPlayer(), world, frameTime);
}
