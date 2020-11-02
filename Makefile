OBJS = .\src\main.c .\src\gui.c .\src\move.c .\src\init.c .\src\utilities.c .\src\server.c .\src\client.c .\src\thread.c

CC = gcc

INCLUDE_PATHS = -IC:\MinGW\include\SDL2

LIBRARY_PATHS = -LC:\MinGW\lib

COMPILER_FLAGS = -w -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net -liphlpapi

OBJ_NAME = Bulltricker

all: $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm -r $(OBJ_NAME)
