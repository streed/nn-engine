# nn-engine
Let's make a game...with raycasting and pain

Dependencies
============

Built using `c++11` ontop of `SDL2`.

Linux
=====
```
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2ttf-dev
```

OSX
===
```
brew install sdl2
brew install sdl2-ttf
```

Building
========
```
make all
```

Running
=======
```
./play
```

If you want to run in fullscreen pass a parameter to the executable.
```
./play 1
```

Controls
========

WASD - Movement
KL - Rotation
Q - Quit
P - Show current FPS (Game loop currently caps at 35FPS).
