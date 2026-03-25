#ifndef INPUT_PACKET_H
#define INPUT_PACKET_H

class InputPacket {
  public:
    bool forward;
    bool backward;
    bool strafeLeft;
    bool strafeRight;
    bool rotateLeft;
    bool rotateRight;
    bool quit;
    bool debug;
    bool shoot;
    bool weapon1;
    bool weapon2;
    bool weapon3;
    bool weapon4;
    bool interact;

    InputPacket(bool forward, bool backward, bool strafeLeft, bool strafeRight,
                bool rotateLeft, bool rotateRight, bool quit, bool debug = false,
                bool shoot = false, bool weapon1 = false, bool weapon2 = false,
                bool weapon3 = false, bool weapon4 = false, bool interact = false);
};

#endif
