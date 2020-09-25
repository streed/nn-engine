OBJS = main.cpp camera.cpp world.cpp color_rgb.cpp input_packet.cpp player.cpp renderer.cpp game.cpp raycast.cpp png.cpp texture.cpp

OBJ_NAME = game


all : $(OBJS)
	g++ $(OBJS) -w -lSDL2 -o $(OBJ_NAME)
