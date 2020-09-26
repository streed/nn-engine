#include "globals.h"
#include "input_packet.h"

InputPacket::InputPacket(bool f,
                         bool b,
                         bool sl,
                         bool sr,
                         bool rl,
                         bool rr,
                         bool q,
                         bool d): forward(f),
                                  backward(b),
                                  strafeLeft(sl),
                                  strafeRight(sr),
                                  rotateLeft(rl),
                                  rotateRight(rr),
                                  quit(q),
                                  debug(d) {}

