#include <stdio.h>
#include <stdlib.h>
#include "initMat.h"

int **initMat(int *rc, int *cc, int *kc, int *qc, int *pc)
{
  int **im;
  FILE *fptr = fopen("settings.txt", "r");
  fscanf(fptr, "%d %d %d %d %d", rc, cc, kc, qc, pc);
  im = malloc((*rc) * sizeof(int*));
  for(int i = 0; i < *rc; i++)
  {
    im[i] = malloc((*cc) * sizeof(int));
  }
  for(int i = 0; i < *rc; i++)
  {
    for(int j = 0; j < *cc; j++)
    {
      fscanf(fptr, "%d", *(im + i) + j);
    }
  }
  fclose(fptr);
  return im;
}
