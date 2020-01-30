#ifdef _WIN32
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "init_mat.h"
#include "gui.h"
#include "move.h"

int main(int argc, char **argv)
{
  #ifdef _WIN32
    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_HIDE );
  #endif
  int **actual_matrix = initMat();
  createBoard(&actual_matrix);
  return 0;
}
