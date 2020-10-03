#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "utilities.h"

int *initialize_board() {
  char *path = "Bulltricker_Data/general.txt";
  FILE *fptr = fopen(path, "r");
  if (fptr == NULL) {
    log_text("\tFailed to open file at <%s>. Critical Error.\n", path);
    return NULL;
  }
  fscanf(fptr, "%d %d %d %d %d", &row_count, &column_count, &WHITE_PIECES_COUNT, &BLACK_PIECES_COUNT, &COLOR_DIVIDER);
  initial_board = malloc(get_board_size()*sizeof(int));
  for (int i = 0; i < get_board_size(); i++)
    fscanf(fptr, "%d, ", &(initial_board[i]));
  fclose(fptr);
  max_states = 10;
  current_states = -1;
  previous_states = malloc(max_states*sizeof(int*));
  for (int i = 0; i < max_states; i++)
    previous_states[i] = malloc(get_board_size()*sizeof(int));
  return initial_board;
}

int get_board_size() {
  if (&row_count != NULL && &column_count != NULL && row_count != 0 && column_count != 0)
    return row_count * column_count;
}
