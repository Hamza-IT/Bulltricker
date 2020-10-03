#ifndef INIT_H
#define INIT_H

typedef enum {
  BLACK_PLAYER = -1,
  WHITE_PLAYER = 1
} Player;

typedef enum {
  BLOCKED = -1,
  EMPTY_NORMAL = 0,
  EMPTY_KING = -2,
  WHITE_PAWN = 11,
  WHITE_QUEEN = 12,
  WHITE_KING = 13,
  BLACK_PAWN = -11,
  BLACK_QUEEN = -12,
  BLACK_KING = -13
} CellType;

typedef enum {
  BLACK_PIECE = -1,
  WHITE_PIECE = 1
} PieceColor;

int WHITE_PIECES_COUNT, BLACK_PIECES_COUNT, COLOR_DIVIDER;
int row_count, column_count, current_states, max_states;
int *initial_board, **previous_states;
Player current_player;

int *initialize_board();
int get_board_size();

#endif
