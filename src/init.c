#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#if defined __unix__ || defined __APPLE__
#include <string.h>
#include <unistd.h>
#include <limits.h>
#endif

int *initialize_board() {
  #if defined __unix__ || defined __APPLE__
    char *target_dir = malloc(PATH_MAX+1);
    #ifdef __unix__
      size_t bytes = readlink("/proc/self/exe", target_dir, PATH_MAX);
      if (bytes >= 0)
        target_dir[bytes+1] = '\0';
      else
        return NULL;
    #elif defined __APPLE__
      size_t max_size = PATH_MAX;
      if (_NSGetExecutablePath(target_dir, &max_size) != 0)
        return NULL;
    #endif
    int rm_index = 0, tmp = 0;
    char *cpy = target_dir;
    while(*cpy != '\0') {
        if (*cpy == '/') {
          rm_index += tmp;
          tmp = 0;
        }
        tmp++;
        cpy++;
    }
    if (rm_index != 0)
      memset(target_dir+rm_index, 0x00, strlen(target_dir)-rm_index);
    chdir(target_dir);
    free(target_dir);
  #endif
  char *path = "./Bulltricker_Data/general.txt";
  FILE *fptr = fopen(path, "r");
  if (fptr == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to open file at <%s>. Critical Error.\n", path);
    return NULL;
  }
  fscanf(fptr, "%d %d %d %d %d", &row_count, &column_count, &WHITE_PIECES_COUNT, &BLACK_PIECES_COUNT, &COLOR_DIVIDER);
  initial_board = malloc(get_board_size()*sizeof(int));
  msg_control = malloc(get_board_size()*sizeof(char *));
  game_input_text = game_output_text = NULL;
  for (int i = 0; i < get_board_size(); i++) {
    fscanf(fptr, "%d, ", &(initial_board[i]));
  }
  for (int i = 0; i < get_board_size(); i++) {
    msg_control[i] = malloc(6);
    fscanf(fptr, "%s ", msg_control[i]);
    msg_control[i][5] = '\0';
  }
  fclose(fptr);
  max_states = 10;
  current_states = -1;
  previous_states = malloc(max_states*sizeof(int*));
  for (int i = 0; i < max_states; i++)
    previous_states[i] = malloc(get_board_size()*sizeof(int));
  return initial_board;
}

int get_board_size() {
  return row_count * column_count;
}
