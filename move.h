#ifndef MOVE_H
#define MOVE_H

typedef enum _bool bool;
int actual_player;
bool check_move(int, int, int, int, int **);
void apply_move(int, int, int, int, int ***);

#endif
