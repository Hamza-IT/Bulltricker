compile:
	gcc main.c game_objects.c init_mat.c -o main
compile_gui:
	gcc main.c game_objects.c init_mat.c gui.c -o main -lSDL2main -lSDL2
clean:
	rm -r main
