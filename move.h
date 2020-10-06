#ifndef MOVE_H
#define MOVE_H

typedef enum {
  FALSE,
  TRUE
} Bool;

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

typedef struct {
  int x;
  int y;
} IntTuple;

typedef struct {
  float x;
  float y;
} FloatTuple;

int allowed_count, allowed_piece_count, *current_allowed_piece;
Bool mandatory_pawn_move, mandatory_queen_move;
IntTuple *current_allowed;

CellState validate_cell(int, int, int *);
GameState check_game_state(int *);
void get_allowed_actions(int *);
void get_allowed_piece_actions(int, int *);
GameState switch_turn(int **, FloatTuple);
GameState apply_move(IntTuple, int **, FloatTuple);
void undo_last_move(int **, Bool *, FloatTuple);

#endif
