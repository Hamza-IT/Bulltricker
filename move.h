#ifndef MOVE_H
#define MOVE_H

typedef enum _bool bool;
typedef enum _mstate mstate;
int actual_player;
int check_mat(int **);
int *check_queen(int **, int, int *);
mstate check_move(int, int, int, int, int **);
void apply_move(int, int, int, int, int ***);

#endif
