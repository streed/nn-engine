#include "input/input_packet.h"

InputPacket::InputPacket(bool f, bool b, bool sl, bool sr,
                         bool rl, bool rr, bool q, bool d,
                         bool shoot, bool w1, bool w2, bool w3, bool w4,
                         bool interact)
  : forward(f), backward(b), strafeLeft(sl), strafeRight(sr),
    rotateLeft(rl), rotateRight(rr), quit(q), debug(d),
    shoot(shoot), weapon1(w1), weapon2(w2), weapon3(w3), weapon4(w4),
    interact(interact) {}
