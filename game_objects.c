#include "game_objects.h"

typedef enum _bool
{
  true = 1,
  false = 0
} bool;

typedef struct _piece
{
  int code;
  int row_pos;
  int column_pos;
  int rotation;
} piece;

typedef struct _tile
{
  int code;
  int row_pos;
  int column_pos;
  bool free;
} tile;
