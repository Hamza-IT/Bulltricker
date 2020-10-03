#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "gui.h"

int main(int argc, char **argv) {
  int *current_board = initialize_board();
  start_game(&current_board);
  return 0;
}
