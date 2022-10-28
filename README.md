# nn-engine Let's make a game...with raycasting and pain

![Example Rendering](https://i.imgur.com/vP9CuwG.jpg)

Dependencies
============

Built using `c++17` ontop of `SDL2`.

Linux
=====
```
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-ttf-dev
sudo apt-get install libboost-all-dev
```

OSX
===
```
brew install sdl2
brew install sdl2-ttf
```

Building
========

Prerequisites:

- cmake >= 3.1
- conan

```
sudo apt-get install cmake
virtualenv -p python3 conan-env
source conan-env/bin/activate
pip install conan
```

Building the code:

```
cd build
conan install ..
cmake ..
make
```

Running
=======
```
cd build/game
./Wenkenstein
```

Controls
========

WASD - Movement
KL - Rotation
Q - Quit
P - Show current FPS (Game loop currently caps at 35FPS).
