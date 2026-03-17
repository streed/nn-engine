#include <memory>

#include "systems/input_system.h"
#include "engine/engine.h"
#include "input/keyboard.h"
#include "input/input_packet.h"
#include "components.h"
#include "coordinator.h"

namespace NN::Systems::BuiltIns {
  void InputSystem::update(NN::Engine *engine, double frameTime) {
    NN::Coordinator *coordinator = engine->getCoordinator();

    auto &input = coordinator->getComponent<Components::Input>(engine->getCurrentPlayer());

    std::unique_ptr<InputPacket> inputPacket(Keyboard::get().getInput());

    input.forward = inputPacket->forward;
    input.backward = inputPacket->backward;
    input.strafeLeft = inputPacket->strafeLeft;
    input.strafeRight = inputPacket->strafeRight;
    input.rotateLeft = inputPacket->rotateLeft;
    input.rotateRight = inputPacket->rotateRight;
    input.quit = inputPacket->quit;
    input.debug = inputPacket->debug;
    input.shoot = inputPacket->shoot;
    input.weapon1 = inputPacket->weapon1;
    input.weapon2 = inputPacket->weapon2;
    input.weapon3 = inputPacket->weapon3;
    input.weapon4 = inputPacket->weapon4;
  }
}
