OBJS = ./src/main.c ./src/gui.c ./src/move.c ./src/init.c ./src/utilities.c ./src/server.c ./src/client.c ./src/thread.c

CC = gcc

# Windows Dependant Variables
ifeq ($(OS), Windows_NT)
	SDL_SYS_FLAGS := -IC:/MinGW/include/SDL2 -LC:/MinGW/lib -w -Wl,-subsystem,windows
	SDL_LINKER_FLAGS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net -liphlpapi

# OS X/GNU/Linux Dependant Variables
else
	SDL_SYS_FLAGS := -w $(shell sdl2-config --cflags)
	SDL_LINKER_FLAGS := $(shell sdl2-config --libs) -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net
endif

OBJ_NAME = Bulltricker

all: $(OBJS)
	$(CC) $(OBJS) $(SDL_SYS_FLAGS) $(SDL_LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm -r $(OBJ_NAME).?*
