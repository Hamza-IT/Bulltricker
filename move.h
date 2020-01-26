#ifndef MOVE_H
#define MOVE_H

typedef enum _bool
{
  false,
  true
} bool;

typedef enum _winner
{
  none,
  whites,
  blacks
} winner;

typedef enum _mstate
{
  impossible,
  normal,
  turn,
  destroy110,
  destroy111,
  destroy210,
  destroy211,
  destroy
} mstate;

int actual_player;
winner check_mat(int **);
int *check_mandatory_move(int **, int, int *);
mstate check_move(int, int, int, int, int **);
void apply_move(int, int, int, int, int ***);

#endif
