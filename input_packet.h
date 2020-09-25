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
    
    InputPacket(bool forward, bool backward, bool strafeLeft, bool strafeRight, bool rotateLeft, bool rotateRight, bool quit);
};

#endif
