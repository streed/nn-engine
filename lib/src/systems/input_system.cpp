#include <iostream>
using namespace std;

#include <boost/scoped_ptr.hpp>

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

    boost::scoped_ptr<InputPacket> inputPacket(Keyboard::get().getInput());

    input.forward = inputPacket.get()->forward;
    input.backward = inputPacket.get()->backward;
    input.strafeLeft = inputPacket.get()->strafeLeft;
    input.strafeRight = inputPacket.get()->strafeRight;
    input.rotateLeft = inputPacket.get()->rotateLeft;
    input.rotateRight = inputPacket.get()->rotateRight;
    input.quit = inputPacket.get()->quit;
    input.debug = inputPacket.get()->debug;
    input.shoot = inputPacket.get()->shoot;
    input.weapon1 = inputPacket.get()->weapon1;
    input.weapon2 = inputPacket.get()->weapon2;
    input.weapon3 = inputPacket.get()->weapon3;
    input.weapon4 = inputPacket.get()->weapon4;
  }
}
