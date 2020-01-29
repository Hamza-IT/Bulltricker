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
  destroy_max11,
  destroy_max21,
  destroy
} mstate;

int actual_player;
bool pawn_mandatory, queen_mandatory;
int mandatory_x, mandatory_y;
winner check_mat(int **);
bool check_null(int **);
void check_pawn(int **);
void check_queen(int **);
mstate check_move(int, int, int, int, int **);
void apply_move(int, int, int, int, int ***);

#endif
