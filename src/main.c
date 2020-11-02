#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "gui.h"

int main(int argc, char **argv) {
  current_board = initialize_board();
  start_game();
  return 0;
}
