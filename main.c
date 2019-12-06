#include <stdio.h>
#include <stdlib.h>
#include "game_objects.h"
#include "init_mat.h"
#include "gui.h"

int main(int argc, char **argv)
{
  int **actual_matrix = initMat();
  createWindow();
  return 0;
}
