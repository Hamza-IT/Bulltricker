//#define _WIN32_WINNT 0x0500
//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "init_mat.h"
#include "gui.h"
#include "move.h"

int main(int argc, char **argv)
{
  //HWND hWnd = GetConsoleWindow();
  //ShowWindow( hWnd, SW_MINIMIZE );
  //ShowWindow( hWnd, SW_HIDE );
  int **actual_matrix = initMat();
  createBoard(actual_matrix);
  return 0;
}
