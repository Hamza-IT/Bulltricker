#ifndef MOVE_H
#define MOVE_H

#include "utilities.h"

typedef enum {
  OUT_OF_BOUNDS,
  EMPTY,
  FRIENDLY,
  ENEMY
} CellState;

typedef enum {
  ONGOING,
  WHITE_WIN,
  BLACK_WIN,
  DRAW
} GameState;

int allowed_count, allowed_piece_count, *current_allowed_piece;
Bool mandatory_pawn_move, mandatory_queen_move;
IntTuple *current_allowed;

CellState validate_cell(int, int);
GameState check_game_state();
void get_allowed_actions();
void get_allowed_piece_actions(int);
GameState switch_turn();
GameState apply_move(IntTuple);
void undo_last_move();

#endif
