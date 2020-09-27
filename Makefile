OBJS = main.cpp camera.cpp world.cpp color_rgb.cpp input_packet.cpp player.cpp renderer.cpp game.cpp raycast.cpp png.cpp texture.cpp config.cpp sprite.cpp monster.cpp

OBJ_NAME = play


all : $(OBJS)
	g++ $(OBJS) -g -w -lSDL2 -l SDL2_ttf -o $(OBJ_NAME)
