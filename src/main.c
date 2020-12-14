#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "gui.h"

int main(int argc, char **argv) {
  set_dir_path();
  if ((current_board = initialize_board()) != NULL) {
    start_game();
    free(current_board);
    for (int i = 0; i < get_board_size(); i++)
      free(msg_control[i]);
    free(initial_board);
    free(msg_control);
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}
