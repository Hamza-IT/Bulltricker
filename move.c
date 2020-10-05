#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "gui.h"
#include "utilities.h"

CellState validate_cell(int current_cell, int cell_to_check, int *current_board) {
  if (cell_to_check < 0 || cell_to_check >= get_board_size())
    return OUT_OF_BOUNDS;
  if ((cell_to_check-current_cell) % column_count != 0 && (cell_to_check < (current_cell/column_count)*column_count || cell_to_check >= (current_cell/column_count+1)*column_count))
    return OUT_OF_BOUNDS;
  if (current_board[cell_to_check] == EMPTY_NORMAL || current_board[cell_to_check] == BLOCKED || current_board[cell_to_check] == EMPTY_KING)
    return EMPTY;
  if (current_board[cell_to_check] / COLOR_DIVIDER == current_board[current_cell] / COLOR_DIVIDER)
    return FRIENDLY;
  else
    return ENEMY;
}

GameState check_game_state(int *current_board) {
  for (int i = 0; i < get_board_size(); i++) {
    if (current_board[i] == WHITE_KING || current_board[i] == BLACK_KING) {
      int surrounding_pieces = 0;
      int opponent_pieces = 0;
      if (validate_cell(i, i-column_count, current_board) != EMPTY)
        surrounding_pieces++;
        if (validate_cell(i, i-column_count, current_board) == ENEMY)
          opponent_pieces++;
      if (validate_cell(i, i+column_count, current_board) != EMPTY)
        surrounding_pieces++;
        if (validate_cell(i, i+column_count, current_board) == ENEMY)
          opponent_pieces++;
      if (validate_cell(i, i-1, current_board) != EMPTY)
        surrounding_pieces++;
        if (validate_cell(i, i-1, current_board) == ENEMY)
          opponent_pieces++;
      if (validate_cell(i, i+1, current_board) != EMPTY)
        surrounding_pieces++;
        if (validate_cell(i, i+1, current_board) == ENEMY)
          opponent_pieces++;
      if (surrounding_pieces == 4 && opponent_pieces > 0) {
        GameState state = current_board[i]/COLOR_DIVIDER == BLACK_PIECE ? WHITE_WIN : BLACK_WIN;
        char winner[50] = "\n\t---------- Game Ended with a ";
        log_text(strcat(winner, state == WHITE_WIN ? "Whites Win ----------\n\n" : "Blacks Win ----------\n\n"));
        return current_board[i]/COLOR_DIVIDER == BLACK_PIECE ? WHITE_WIN : BLACK_WIN;
      }
    }
  }
  if (allowed_count == 0 || (WHITE_PIECES_COUNT <= 5 && BLACK_PIECES_COUNT <= 5)) {
    log_text("\n\t---------- Game Ended with a Draw ----------\n\n");
    return DRAW;
  }
  return ONGOING;
}

void get_allowed_actions(int *current_board) {
  free(current_allowed);
  IntTuple *allowed = malloc(get_board_size()*(get_board_size()-1)*sizeof(IntTuple));
  mandatory_pawn_move = FALSE; mandatory_queen_move = FALSE;
  int k = 0;
  for (int i = 0; i < get_board_size(); i++) {
    if (current_board[i] / COLOR_DIVIDER == current_player) {
      if ((current_board[i] == WHITE_PAWN || current_board[i] == BLACK_PAWN) && mandatory_queen_move == FALSE) {
        // Pawn Logic Here
        PieceColor color_mult = current_board[i] / COLOR_DIVIDER; // Piece Color Identification
        if (mandatory_pawn_move == FALSE) {
          if (validate_cell(i-column_count*color_mult, i-(column_count-1)*color_mult, current_board) == EMPTY)
            allowed[k++] = (IntTuple) { i, i-(column_count-1)*color_mult };
          if (validate_cell(i-column_count*color_mult, i-(column_count+1)*color_mult, current_board) == EMPTY)
            allowed[k++] = (IntTuple) { i, i-(column_count+1)*color_mult };
          if (validate_cell(i, i-(column_count*2)*color_mult, current_board) == EMPTY && validate_cell(i, i-column_count*color_mult, current_board) == EMPTY) {
            allowed[k++] = (IntTuple) { i, i-(column_count*2)*color_mult };
            // Pawn Double Starting Step
            if (((color_mult == WHITE_PIECE && i/column_count == row_count-3) || (color_mult == BLACK_PIECE && i/column_count == 2)) && validate_cell(i, i-(column_count*4)*color_mult, current_board) == EMPTY && validate_cell(i, i-(column_count*3)*color_mult, current_board) == EMPTY)
              allowed[k++] = (IntTuple) { i, i-(column_count*4)*color_mult };
          }
        }
        // Non Rotated Pawn Identification
        if ((i/column_count) % 2 == 0) {
          IntTuple possible_move = { 0, 0 };
          for (int j = 2; j < row_count; j += 4) {
            if (validate_cell(i, i-(j-1)*column_count*color_mult, current_board) == EMPTY && validate_cell(i, i-j*column_count*color_mult, current_board) == ENEMY && validate_cell(i, i-(j+1)*column_count*color_mult, current_board) == EMPTY && validate_cell(i, i-(j+2)*column_count*color_mult, current_board) == EMPTY) {
              possible_move.x = i; possible_move.y = i-(j+2)*column_count*color_mult;
              if (mandatory_pawn_move == FALSE) {
                for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                k = 0;
              }
              mandatory_pawn_move = TRUE;
            }
            else {
              if (possible_move.x != 0 || possible_move.y != 0)
                allowed[k++] = possible_move;
              break;
            }
          }
        }
      }
      if (current_board[i] == WHITE_QUEEN || current_board[i] == BLACK_QUEEN) {
        // Queen Logic Here
        int l = 0;
        IntTuple *tmp = malloc((get_board_size()-1)*get_board_size()*sizeof(IntTuple));
        Bool checking_up = TRUE, checking_down = TRUE, checking_left = TRUE, checking_right = TRUE;
        Bool mandatory_up = FALSE, mandatory_down = FALSE, mandatory_left = FALSE, mandatory_right = FALSE;
        if (mandatory_queen_move == FALSE && mandatory_pawn_move == FALSE) {
          if (validate_cell(i-column_count, i-(column_count-1), current_board) == EMPTY)
            tmp[l++] = (IntTuple) { i, i-(column_count-1) };
          if (validate_cell(i-column_count, i-(column_count+1), current_board) == EMPTY)
            tmp[l++] = (IntTuple) { i, i-(column_count+1) };
          if (validate_cell(i+column_count, i+(column_count-1), current_board) == EMPTY)
            tmp[l++] = (IntTuple) { i, i+(column_count-1) };
          if (validate_cell(i+column_count, i+(column_count+1), current_board) == EMPTY)
            tmp[l++] = (IntTuple) { i, i+(column_count+1) };
        }
        for (int j = 2; j < row_count; j += 2) {
          if (mandatory_left == FALSE && mandatory_right == FALSE) {
            if (checking_up == TRUE) {
              // Rotated Queen Identification
              if ((i/column_count) % 2 == 1) {
                if (mandatory_queen_move == FALSE && mandatory_pawn_move == FALSE) {
                  if (validate_cell(i, i-column_count*j, current_board) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i-column_count*j };
                  else
                    checking_up = FALSE;
                }
              }
              // Non Rotated Queen Identification
              else {
                if (validate_cell(i, i-(j-1)*column_count, current_board) == EMPTY) {
                  if (validate_cell(i, i-j*column_count, current_board) == EMPTY && mandatory_pawn_move == FALSE && (mandatory_queen_move == FALSE || mandatory_up == TRUE))
                    tmp[l++] = (IntTuple) { i, i-column_count*j };
                  else if (validate_cell(i, i-j*column_count, current_board) == ENEMY && validate_cell(i, i-(j+1)*column_count, current_board) == EMPTY && validate_cell(i, i-(j+2)*column_count, current_board) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    if (mandatory_queen_move == FALSE) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_up = TRUE;
                  }
                  else
                    checking_up = FALSE;
                }
                else
                  checking_up = FALSE;
              }
            }
            if (checking_down == TRUE) {
              // Rotated Queen Identification
              if ((i/column_count) % 2 == 1) {
                if (mandatory_queen_move == FALSE && mandatory_pawn_move == FALSE) {
                  if (validate_cell(i, i+column_count*j, current_board) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i+column_count*j };
                  else
                    checking_down = FALSE;
                }
              }
              // Non Rotated Queen Identification
              else {
                if (validate_cell(i, i+(j-1)*column_count, current_board) == EMPTY) {
                  if (validate_cell(i, i+j*column_count, current_board) == EMPTY && mandatory_pawn_move == FALSE && (mandatory_queen_move == FALSE || mandatory_down == TRUE))
                    tmp[l++] = (IntTuple) { i, i+column_count*j };
                  else if (validate_cell(i, i+j*column_count, current_board) == ENEMY && validate_cell(i, i+(j+1)*column_count, current_board) == EMPTY && validate_cell(i, i+(j+2)*column_count, current_board) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    if (mandatory_queen_move == FALSE) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_down = TRUE;
                  }
                  else
                    checking_down = FALSE;
                }
                else
                  checking_down = FALSE;
              }
            }
          }
          if (mandatory_up == FALSE && mandatory_down == FALSE) {
            if (checking_left == TRUE) {
              // Non Rotated Queen Identification
              if ((i/column_count) % 2 == 0) {
                if (mandatory_queen_move == FALSE && mandatory_pawn_move == FALSE) {
                  if (validate_cell(i, i-j, current_board) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i-j };
                  else
                    checking_left = FALSE;
                }
              }
              // Rotated Queen Identification
              else {
                if (validate_cell(i, i-(j-1), current_board) == EMPTY) {
                  if (validate_cell(i, i-j, current_board) == EMPTY && mandatory_pawn_move == FALSE && (mandatory_queen_move == FALSE || mandatory_left == TRUE))
                    tmp[l++] = (IntTuple) { i, i-j };
                  else if (validate_cell(i, i-j, current_board) == ENEMY && validate_cell(i, i-(j+1), current_board) == EMPTY && validate_cell(i, i-(j+2), current_board) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    if (mandatory_queen_move == FALSE) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_left = TRUE;
                  }
                  else
                    checking_left = FALSE;
                }
                else
                  checking_left = FALSE;
              }
            }
            if (checking_right == TRUE) {
              // Non Rotated Queen Identification
              if ((i/column_count) % 2 == 0) {
                if (mandatory_queen_move == FALSE && mandatory_pawn_move == FALSE) {
                  if (validate_cell(i, i+j, current_board) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i+j };
                  else
                    checking_right = FALSE;
                }
              }
              // Rotated Queen Identification
              else {
                if (validate_cell(i, i+(j-1), current_board) == EMPTY) {
                  if (validate_cell(i, i+j, current_board) == EMPTY && mandatory_pawn_move == FALSE && (mandatory_queen_move == FALSE || mandatory_right == TRUE))
                    tmp[l++] = (IntTuple) { i, i+j };
                  else if (validate_cell(i, i+j, current_board) == ENEMY && validate_cell(i, i+(j+1), current_board) == EMPTY && validate_cell(i, i+(j+2), current_board) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    if (mandatory_queen_move == FALSE) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_right = TRUE;
                  }
                  else
                    checking_right = FALSE;
                }
                else
                  checking_right = FALSE;
              }
            }
          }
        }
        for (int j = 0; j < l; j++)
          allowed[k++] = tmp[j];
      }
      if (current_board[i] == WHITE_KING || current_board[i] == BLACK_KING) {
        // King Logic Here
        if (mandatory_pawn_move == FALSE && mandatory_queen_move == FALSE) {
          if (validate_cell(i, i+(column_count*2), current_board) == EMPTY && validate_cell(i, i+column_count, current_board) == EMPTY && (validate_cell(i, i+(column_count*4), current_board) == OUT_OF_BOUNDS || validate_cell(i, i+(column_count*4), current_board) == EMPTY) && (validate_cell(i+(column_count*4), i+(column_count*4+2), current_board) == OUT_OF_BOUNDS || validate_cell(i+(column_count*4), i+(column_count*4+2), current_board) == EMPTY) && (validate_cell(i+(column_count*4), i+(column_count*4-2), current_board) == OUT_OF_BOUNDS || validate_cell(i+(column_count*4), i+(column_count*4-2), current_board) == EMPTY))
            allowed[k++] = (IntTuple) { i, i+(column_count*2) };
          if (validate_cell(i, i-(column_count*2), current_board) == EMPTY && validate_cell(i, i-column_count, current_board) == EMPTY && (validate_cell(i, i-(column_count*4), current_board) == OUT_OF_BOUNDS || validate_cell(i, i-(column_count*4), current_board) == EMPTY) && (validate_cell(i-(column_count*4), i-(column_count*4-2), current_board) == OUT_OF_BOUNDS || validate_cell(i-(column_count*4), i-(column_count*4-2), current_board) == EMPTY) && (validate_cell(i-(column_count*4), i-(column_count*4+2), current_board) == OUT_OF_BOUNDS || validate_cell(i-(column_count*4), i-(column_count*4+2), current_board) == EMPTY))
            allowed[k++] = (IntTuple) { i, i-(column_count*2) };
          if (validate_cell(i, i+2, current_board) == EMPTY && validate_cell(i, i+1, current_board) == EMPTY && (validate_cell(i, i+4, current_board) == OUT_OF_BOUNDS || validate_cell(i, i+4, current_board) == EMPTY) && (validate_cell(i+column_count, i+4+column_count, current_board) == OUT_OF_BOUNDS || validate_cell(i+column_count, i+4+column_count, current_board) == EMPTY) && (validate_cell(i-column_count, i+4-column_count, current_board) == OUT_OF_BOUNDS || validate_cell(i-column_count, i+4-column_count, current_board) == EMPTY))
            allowed[k++] = (IntTuple) { i, i+2 };
          if (validate_cell(i, i-2, current_board) == EMPTY && validate_cell(i, i-1, current_board) == EMPTY && (validate_cell(i, i-4, current_board) == OUT_OF_BOUNDS || validate_cell(i, i-4, current_board) == EMPTY) && (validate_cell(i+column_count, i-4+column_count, current_board) == OUT_OF_BOUNDS || validate_cell(i+column_count, i-4+column_count, current_board) == EMPTY) && (validate_cell(i-column_count, i-4-column_count, current_board) == OUT_OF_BOUNDS || validate_cell(i-column_count, i-4-column_count, current_board) == EMPTY))
            allowed[k++] = (IntTuple) { i, i-2 };
        }
      }
    }
  }
  allowed_count = k;
  current_allowed = malloc(allowed_count*sizeof(IntTuple));
  for (int i = 0; i < allowed_count; i++) { current_allowed[i] = allowed[i]; }
}

GameState switch_turn(int *current_board, FloatTuple offset) {
  current_player = -current_player;
  get_allowed_actions(current_board);
  draw(current_board, offset, FALSE);
  return check_game_state(current_board);
}

void get_allowed_piece_actions(int index, int *current_board) {
  free(current_allowed_piece);
  if (current_allowed == NULL)
    get_allowed_actions(current_board);
  int *allowed_actions = malloc(sizeof(int)*allowed_count);
  allowed_piece_count = 0;
  for (int i = 0; i < allowed_count; i++) {
    if (current_allowed[i].x == index) {
      allowed_actions[allowed_piece_count] = current_allowed[i].y;
      allowed_piece_count++;
    }
  }
  current_allowed_piece = malloc(sizeof(int)*allowed_piece_count);
  for (int i = 0; i < allowed_piece_count; i++) { current_allowed_piece[i] = allowed_actions[i]; }
}

GameState apply_move(IntTuple move, int **current_board, FloatTuple offset) {
  if ((*current_board)[move.x]/COLOR_DIVIDER == current_player) {
    current_states++;
    if (current_states == max_states) {
      for (int i = 0; i < current_states-1; i++) {
        for (int j = 0; j < get_board_size(); j++) {
          (previous_states[i])[j] = (previous_states[i+1])[j];
        }
      }
      current_states--;
    }
    for (int i = 0; i < get_board_size(); i++) { (previous_states[current_states])[i] = (*current_board)[i]; }
    int origin_piece = (*current_board)[move.x];
    // King Action
    if ((*current_board)[move.x] == WHITE_KING || (*current_board)[move.x] == BLACK_KING)
      (*current_board)[move.x] = -2;
    // Pawn/Queen Action
    else {
      int last_position = move.x;
      int distance = move.y-move.x;
      int direction = distance/abs(distance);
      (*current_board)[move.x] = 0;
      if (distance % column_count == 0) {
        for (int i = column_count*2*direction; abs(i) < abs(distance); i += column_count*2*direction) {
          if (validate_cell(last_position, move.x+i, *current_board) == ENEMY) {
            if ((*current_board)[move.x+i]/COLOR_DIVIDER == WHITE_PLAYER)
              WHITE_PIECES_COUNT--;
            else
              BLACK_PIECES_COUNT--;
            (*current_board)[last_position] = 0;
            last_position = move.x+i+column_count*2*direction;
            (*current_board)[last_position] = origin_piece;
            (*current_board)[move.x+i] = 0;
            draw(*current_board, offset, FALSE);
            play_sound(sounds[MOVE2]);
            SDL_Delay(400);
          }
        }
      }
      // Queen Horizontal Movement
      else if ((origin_piece == WHITE_QUEEN || origin_piece == BLACK_QUEEN) && validate_cell(move.x, move.y, *current_board) != OUT_OF_BOUNDS) {
        for (int i = 2*direction; abs(i) < abs(distance); i += 2*direction) {
          if (validate_cell(last_position, move.x+i, *current_board) == ENEMY) {
            if ((*current_board)[move.x+i]/COLOR_DIVIDER == WHITE_PLAYER)
              WHITE_PIECES_COUNT--;
            else
              BLACK_PIECES_COUNT--;
            (*current_board)[last_position] = 0;
            last_position = move.x+i+2*direction;
            (*current_board)[last_position] = origin_piece;
            (*current_board)[move.x+i] = 0;
            draw(*current_board, offset, FALSE);
            play_sound(sounds[MOVE2]);
            SDL_Delay(400);
          }
        }
      }
    }
    // Pawn Promotion Check
    if ((origin_piece == WHITE_PAWN && move.y/column_count == 0) || (origin_piece == BLACK_PAWN && move.y/column_count == row_count-1)) {
      PieceColor color_mult = origin_piece/COLOR_DIVIDER;
      (*current_board)[move.y] = origin_piece;
      draw(*current_board, offset, FALSE);
      SDL_Delay(300);
      play_sound(sounds[PUP]);
      (*current_board)[move.y] = WHITE_QUEEN*color_mult;
    }
    else if ((*current_board)[move.y] != origin_piece) {
      (*current_board)[move.y] = origin_piece;
      play_sound(sounds[MOVE2]);
    }
    if (current_player == WHITE_PLAYER) {
      char player[50] = "\t\tWhites Move: ";
      log_text(strcat(player, "(%d, %d)\n\t\tWhite Pieces Left: %d\tBlack Pieces Left: %d\n\n"), move.x, move.y, WHITE_PIECES_COUNT, BLACK_PIECES_COUNT);
    }
    else {
      char player[50] = "\t\tBlacks Move: ";
      log_text(strcat(player, "(%d, %d)\n\t\tWhite Pieces Left: %d\tBlack Pieces Left: %d\n\n"), move.x, move.y, WHITE_PIECES_COUNT, BLACK_PIECES_COUNT);
    }
    return switch_turn(*current_board, offset);
  }
}

void undo_last_move(int **current_board, Bool *game_over, FloatTuple offset) {
  if (current_states >= 0) {
    for (int i = 0; i < get_board_size(); i++)
      (*current_board)[i] = (previous_states[current_states])[i];
    current_states--;
    switch_turn(*current_board, offset);
    *game_over = FALSE;
    play_sound(sounds[BIP2]);
    char log[50] = "\t\tUndoing last move. ";
    log_text(strcat(log, current_player == WHITE_PLAYER ? "Whites turn to play.\n" : "Blacks turn to play.\n"));
  }
}
