OBJS = main.cpp

OBJ_NAME = game


all : $(OBJS)
	g++ $(OBJS) -w -lSDL2 -o $(OBJ_NAME)
