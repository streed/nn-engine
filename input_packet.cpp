#include "globals.h"
#include "input_packet.h"

InputPacket::InputPacket( bool f, bool b, bool sl, bool sr, bool rl, bool rr, bool q) {
  this->forward = f;
  this->backward = b;
  this->strafeLeft = sl;
  this->strafeRight = sr;
  this->rotateLeft = rl;
  this->rotateRight = rr;
  this->quit = q;
}

