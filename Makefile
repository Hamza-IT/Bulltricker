compile:
	gcc main.c game_objects.c init_mat.c -o Bulltricker -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
clean:
	rm -r Bulltricker
