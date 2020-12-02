#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "gui.h"
#include "move.h"
#include "thread.h"

CellState validate_cell(int current_cell, int cell_to_check) {
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

GameState check_game_state() {
  for (int i = 0; i < get_board_size(); i++) {
    if (current_board[i] == WHITE_KING || current_board[i] == BLACK_KING) {
      int surrounding_pieces = 0;
      int opponent_pieces = 0;
      if (validate_cell(i, i-column_count) != EMPTY) {
        surrounding_pieces++;
        if (validate_cell(i, i-column_count) == ENEMY)
          opponent_pieces++;
      }
      if (validate_cell(i, i+column_count) != EMPTY) {
        surrounding_pieces++;
        if (validate_cell(i, i+column_count) == ENEMY)
          opponent_pieces++;
      }
      if (validate_cell(i, i-1) != EMPTY) {
        surrounding_pieces++;
        if (validate_cell(i, i-1) == ENEMY)
          opponent_pieces++;
      }
      if (validate_cell(i, i+1) != EMPTY) {
        surrounding_pieces++;
        if (validate_cell(i, i+1) == ENEMY)
          opponent_pieces++;
      }
      if (surrounding_pieces == 4 && opponent_pieces > 0) {
        GameState state = current_board[i]/COLOR_DIVIDER == BLACK_PIECE ? WHITE_WIN : BLACK_WIN;
        char winner[50] = "\t---------- Game Ended with a ";
        debug_log(true, __LINE__, __FILE__, strcat(winner, state == WHITE_WIN ? "Whites Win ----------\n\n" : "Blacks Win ----------\n\n"));
        return current_board[i]/COLOR_DIVIDER == BLACK_PIECE ? WHITE_WIN : BLACK_WIN;
      }
    }
  }
  if (allowed_count == 0 || (WHITE_PIECES_COUNT < 5 && BLACK_PIECES_COUNT < 5)) {
    debug_log(true, __LINE__, __FILE__, "\t---------- Game Ended with a Draw ----------\n\n");
    return DRAW;
  }
  return ONGOING;
}

void get_allowed_actions() {
  free(current_allowed);
  IntTuple *allowed = malloc(get_board_size()*(get_board_size()-1)*sizeof(IntTuple));
  mandatory_pawn_move = false; mandatory_queen_move = false;
  int k = 0;
  for (int i = 0; i < get_board_size(); i++) {
    if (current_board[i] / COLOR_DIVIDER == current_player) {
      if ((current_board[i] == WHITE_PAWN || current_board[i] == BLACK_PAWN) && mandatory_queen_move == false) {
        // Pawn Logic Here
        PieceColor color_mult = current_board[i] / COLOR_DIVIDER; // Piece Color Identification
        if (mandatory_pawn_move == false) {
          if (validate_cell(i-column_count*color_mult, i-(column_count-1)*color_mult) == EMPTY)
            allowed[k++] = (IntTuple) { i, i-(column_count-1)*color_mult };
          if (validate_cell(i-column_count*color_mult, i-(column_count+1)*color_mult) == EMPTY)
            allowed[k++] = (IntTuple) { i, i-(column_count+1)*color_mult };
          if (validate_cell(i, i-(column_count*2)*color_mult) == EMPTY && validate_cell(i, i-column_count*color_mult) == EMPTY) {
            allowed[k++] = (IntTuple) { i, i-(column_count*2)*color_mult };
            // Pawn Double Starting Step
            if (((color_mult == WHITE_PIECE && i/column_count == row_count-3) || (color_mult == BLACK_PIECE && i/column_count == 2)) && validate_cell(i, i-(column_count*4)*color_mult) == EMPTY && validate_cell(i, i-(column_count*3)*color_mult) == EMPTY)
              allowed[k++] = (IntTuple) { i, i-(column_count*4)*color_mult };
          }
        }
        // Non Rotated Pawn Identification
        if ((i/column_count) % 2 == 0) {
          IntTuple possible_move = { 0, 0 };
          for (int j = 2; j < row_count; j += 4) {
            if (validate_cell(i, i-(j-1)*column_count*color_mult) == EMPTY && validate_cell(i, i-j*column_count*color_mult) == ENEMY && validate_cell(i, i-(j+1)*column_count*color_mult) == EMPTY && validate_cell(i, i-(j+2)*column_count*color_mult) == EMPTY) {
              possible_move.x = i; possible_move.y = i-(j+2)*column_count*color_mult;
              if (mandatory_pawn_move == false) {
                for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                k = 0;
              }
              mandatory_pawn_move = true;
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
        int l = 0, l_up = 0, l_down = 0, l_left = 0, l_right = 0;
        IntTuple *tmp = malloc(get_board_size()*sizeof(IntTuple)), *tmp_up = malloc(get_board_size()*sizeof(IntTuple)), *tmp_down = malloc(get_board_size()*sizeof(IntTuple));
        IntTuple *tmp_left = malloc(get_board_size()*sizeof(IntTuple)), *tmp_right = malloc(get_board_size()*sizeof(IntTuple));
        Bool checking_up = true, checking_down = true, checking_left = true, checking_right = true;
        Bool mandatory_up = false, mandatory_down = false, mandatory_left = false, mandatory_right = false;
        if (mandatory_queen_move == false && mandatory_pawn_move == false) {
          if (validate_cell(i-column_count, i-(column_count-1)) == EMPTY)
            tmp[l++] = (IntTuple) { i, i-(column_count-1) };
          if (validate_cell(i-column_count, i-(column_count+1)) == EMPTY)
            tmp[l++] = (IntTuple) { i, i-(column_count+1) };
          if (validate_cell(i+column_count, i+(column_count-1)) == EMPTY)
            tmp[l++] = (IntTuple) { i, i+(column_count-1) };
          if (validate_cell(i+column_count, i+(column_count+1)) == EMPTY)
            tmp[l++] = (IntTuple) { i, i+(column_count+1) };
        }
        int max_count = row_count >= column_count ? row_count : column_count;
        for (int j = 2; j < max_count; j += 2) {
          if (mandatory_left == false && mandatory_right == false && j < row_count) {
            if (checking_up == true) {
              // Rotated Queen Identification
              if ((i/column_count) % 2 == 1) {
                if (mandatory_queen_move == false && mandatory_pawn_move == false) {
                  if (validate_cell(i, i-column_count*j) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i-column_count*j };
                  else
                    checking_up = false;
                }
              }
              // Non Rotated Queen Identification
              else {
                if (validate_cell(i, i-(j-1)*column_count) == EMPTY) {
                  if (validate_cell(i, i-j*column_count) == EMPTY) {
                    if (mandatory_up == true)
                      tmp_up[l_up++] = (IntTuple) { i, i-column_count*j };
                    else if (mandatory_pawn_move == false && mandatory_queen_move == false)
                      tmp[l++] = (IntTuple) { i, i-column_count*j };
                  }
                  else if (validate_cell(i, i-j*column_count) == ENEMY && validate_cell(i, i-(j+1)*column_count) == EMPTY && validate_cell(i, i-(j+2)*column_count) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    for (int x = 0; x < l_up; x++) { tmp_up[x] = (IntTuple) { 0, 0 }; }
                    l_up = 0;
                    if (mandatory_queen_move == false) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_up = true;
                  }
                  else {
                    checking_up = false;
                    for (int x = 0; x < l_up; x++)
                      allowed[k++] = tmp_up[x];
                    l_up = 0;
                  }
                }
                else {
                  checking_up = false;
                  for (int x = 0; x < l_up; x++)
                    allowed[k++] = tmp_up[x];
                  l_up = 0;
                }
              }
            }
            if (checking_down == true) {
              // Rotated Queen Identification
              if ((i/column_count) % 2 == 1) {
                if (mandatory_queen_move == false && mandatory_pawn_move == false) {
                  if (validate_cell(i, i+column_count*j) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i+column_count*j };
                  else
                    checking_down = false;
                }
              }
              // Non Rotated Queen Identification
              else {
                if (validate_cell(i, i+(j-1)*column_count) == EMPTY) {
                  if (validate_cell(i, i+j*column_count) == EMPTY) {
                    if (mandatory_down == true)
                      tmp_down[l_down++] = (IntTuple) { i, i+column_count*j };
                    else if (mandatory_pawn_move == false && mandatory_queen_move == false)
                      tmp[l++] = (IntTuple) { i, i+column_count*j };
                  }
                  else if (validate_cell(i, i+j*column_count) == ENEMY && validate_cell(i, i+(j+1)*column_count) == EMPTY && validate_cell(i, i+(j+2)*column_count) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    for (int x = 0; x < l_down; x++) { tmp_down[x] = (IntTuple) { 0, 0 }; }
                    l_down = 0;
                    if (mandatory_queen_move == false) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_down = true;
                  }
                  else {
                    checking_down = false;
                    for (int x = 0; x < l_down; x++)
                      allowed[k++] = tmp_down[x];
                    l_down = 0;
                  }
                }
                else {
                  checking_down = false;
                  for (int x = 0; x < l_down; x++)
                    allowed[k++] = tmp_down[x];
                  l_down = 0;
                }
              }
            }
          }
          if (mandatory_up == false && mandatory_down == false && j < column_count) {
            if (checking_left == true) {
              // Non Rotated Queen Identification
              if ((i/column_count) % 2 == 0) {
                if (mandatory_queen_move == false && mandatory_pawn_move == false) {
                  if (validate_cell(i, i-j) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i-j };
                  else
                    checking_left = false;
                }
              }
              // Rotated Queen Identification
              else {
                if (validate_cell(i, i-(j-1)) == EMPTY) {
                  if (validate_cell(i, i-j) == EMPTY) {
                    if (mandatory_left == true)
                      tmp_left[l_left++] = (IntTuple) { i, i-j };
                    else if (mandatory_pawn_move == false && mandatory_queen_move == false)
                      tmp[l++] = (IntTuple) { i, i-j };
                  }
                  else if (validate_cell(i, i-j) == ENEMY && validate_cell(i, i-(j+1)) == EMPTY && validate_cell(i, i-(j+2)) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    for (int x = 0; x < l_left; x++) { tmp_left[x] = (IntTuple) { 0, 0 }; }
                    l_left = 0;
                    if (mandatory_queen_move == false) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_left = true;
                  }
                  else {
                    checking_left = false;
                    for (int x = 0; x < l_left; x++)
                      allowed[k++] = tmp_left[x];
                    l_left = 0;
                  }
                }
                else {
                  checking_left = false;
                  for (int x = 0; x < l_left; x++)
                    allowed[k++] = tmp_left[x];
                  l_left = 0;
                }
              }
            }
            if (checking_right == true) {
              // Non Rotated Queen Identification
              if ((i/column_count) % 2 == 0) {
                if (mandatory_queen_move == false && mandatory_pawn_move == false) {
                  if (validate_cell(i, i+j) == EMPTY)
                    tmp[l++] = (IntTuple) { i, i+j };
                  else
                    checking_right = false;
                }
              }
              // Rotated Queen Identification
              else {
                if (validate_cell(i, i+(j-1)) == EMPTY) {
                  if (validate_cell(i, i+j) == EMPTY) {
                    if (mandatory_right == true)
                      tmp_right[l_right++] = (IntTuple) { i, i+j };
                    else if (mandatory_pawn_move == false && mandatory_queen_move == false)
                      tmp[l++] = (IntTuple) { i, i+j };
                  }
                  else if (validate_cell(i, i+j) == ENEMY && validate_cell(i, i+(j+1)) == EMPTY && validate_cell(i, i+(j+2)) == EMPTY) {
                    for (int x = 0; x < l; x++) { tmp[x] = (IntTuple) { 0, 0 }; }
                    l = 0;
                    for (int x = 0; x < l_right; x++) { tmp_right[x] = (IntTuple) { 0, 0 }; }
                    l_right = 0;
                    if (mandatory_queen_move == false) {
                      for (int x = 0; x < k; x++) { allowed[x] = (IntTuple) { 0, 0 }; }
                      k = 0;
                    }
                    mandatory_queen_move = mandatory_right = true;
                  }
                  else {
                    checking_right = false;
                    for (int x = 0; x < l_right; x++)
                      allowed[k++] = tmp_right[x];
                    l_right = 0;
                  }
                }
                else {
                  checking_right = false;
                  for (int x = 0; x < l_right; x++)
                    allowed[k++] = tmp_right[x];
                  l_right = 0;
                }
              }
            }
          }
        }
        for (int j = 0; j < l; j++)
          allowed[k++] = tmp[j];
        for (int j = 0; j < l_up; j++)
          allowed[k++] = tmp_up[j];
        for (int j = 0; j < l_down; j++)
          allowed[k++] = tmp_down[j];
        for (int j = 0; j < l_left; j++)
          allowed[k++] = tmp_left[j];
        for (int j = 0; j < l_right; j++)
          allowed[k++] = tmp_right[j];
        free(tmp); free(tmp_up); free(tmp_down);
        free(tmp_left); free(tmp_right);
      }
      if (current_board[i] == WHITE_KING || current_board[i] == BLACK_KING) {
        // King Logic Here
        if (mandatory_pawn_move == false && mandatory_queen_move == false) {
          if (validate_cell(i, i+(column_count*2)) == EMPTY && validate_cell(i, i+column_count) == EMPTY && (validate_cell(i, i+(column_count*4)) == OUT_OF_BOUNDS || validate_cell(i, i+(column_count*4)) == EMPTY) && (validate_cell(i+(column_count*4), i+(column_count*4+2)) == OUT_OF_BOUNDS || validate_cell(i+(column_count*4), i+(column_count*4+2)) == EMPTY) && (validate_cell(i+(column_count*4), i+(column_count*4-2)) == OUT_OF_BOUNDS || validate_cell(i+(column_count*4), i+(column_count*4-2)) == EMPTY))
            allowed[k++] = (IntTuple) { i, i+(column_count*2) };
          if (validate_cell(i, i-(column_count*2)) == EMPTY && validate_cell(i, i-column_count) == EMPTY && (validate_cell(i, i-(column_count*4)) == OUT_OF_BOUNDS || validate_cell(i, i-(column_count*4)) == EMPTY) && (validate_cell(i-(column_count*4), i-(column_count*4-2)) == OUT_OF_BOUNDS || validate_cell(i-(column_count*4), i-(column_count*4-2)) == EMPTY) && (validate_cell(i-(column_count*4), i-(column_count*4+2)) == OUT_OF_BOUNDS || validate_cell(i-(column_count*4), i-(column_count*4+2)) == EMPTY))
            allowed[k++] = (IntTuple) { i, i-(column_count*2) };
          if (validate_cell(i, i+2) == EMPTY && validate_cell(i, i+1) == EMPTY && (validate_cell(i, i+4) == OUT_OF_BOUNDS || validate_cell(i, i+4) == EMPTY) && (validate_cell(i+2*column_count, i+4+2*column_count) == OUT_OF_BOUNDS || validate_cell(i+2*column_count, i+4+2*column_count) == EMPTY) && (validate_cell(i-2*column_count, i+4-2*column_count) == OUT_OF_BOUNDS || validate_cell(i-2*column_count, i+4-2*column_count) == EMPTY))
            allowed[k++] = (IntTuple) { i, i+2 };
          if (validate_cell(i, i-2) == EMPTY && validate_cell(i, i-1) == EMPTY && (validate_cell(i, i-4) == OUT_OF_BOUNDS || validate_cell(i, i-4) == EMPTY) && (validate_cell(i+2*column_count, i-4+2*column_count) == OUT_OF_BOUNDS || validate_cell(i+2*column_count, i-4+2*column_count) == EMPTY) && (validate_cell(i-2*column_count, i-4-2*column_count) == OUT_OF_BOUNDS || validate_cell(i-2*column_count, i-4-2*column_count) == EMPTY))
            allowed[k++] = (IntTuple) { i, i-2 };
        }
      }
    }
  }
  allowed_count = k;
  current_allowed = malloc(allowed_count*sizeof(IntTuple));
  for (int i = 0; i < allowed_count; i++) { current_allowed[i] = allowed[i]; }
}

GameState switch_turn() {
  current_player = -current_player;
  get_allowed_actions();
  draw_board(false, true);
  return check_game_state();
}

void get_allowed_piece_actions(int index) {
  free(current_allowed_piece);
  if (current_allowed == NULL)
    get_allowed_actions();
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

GameState apply_move(IntTuple move) {
  if (current_board[move.x]/COLOR_DIVIDER == current_player) {
    current_states++;
    if (current_states == max_states) {
      for (int i = 0; i < current_states-1; i++) {
        for (int j = 0; j < get_board_size(); j++) {
          (previous_states[i])[j] = (previous_states[i+1])[j];
        }
      }
      current_states--;
    }
    for (int i = 0; i < get_board_size(); i++) { (previous_states[current_states])[i] = current_board[i]; }
    int origin_piece = current_board[move.x];
    // King Action
    if (current_board[move.x] == WHITE_KING || current_board[move.x] == BLACK_KING)
      current_board[move.x] = EMPTY_KING;
    // Pawn/Queen Action
    else {
      int last_position = move.x;
      int distance = move.y-move.x;
      int direction = distance/abs(distance);
      current_board[move.x] = EMPTY_NORMAL;
      if (distance % column_count == 0 || ((origin_piece == WHITE_QUEEN || origin_piece == BLACK_QUEEN) && validate_cell(move.x, move.y) != OUT_OF_BOUNDS)) {
        int step;
        if (distance % column_count == 0)
          step = 2*column_count*direction;
        // Queen Horizontal Movement
        else if ((origin_piece == WHITE_QUEEN || origin_piece == BLACK_QUEEN) && validate_cell(move.x, move.y) != OUT_OF_BOUNDS)
          step = 2*direction;
        for (int i = step; abs(i) < abs(distance); i += step) {
          if (validate_cell(last_position, move.x+i) == ENEMY) {
            if (current_board[move.x+i]/COLOR_DIVIDER == WHITE_PLAYER)
              WHITE_PIECES_COUNT--;
            else
              BLACK_PIECES_COUNT--;
            current_board[last_position] = EMPTY_NORMAL;
            last_position = move.x+i+step;
            current_board[last_position] = origin_piece;
            current_board[move.x+i] = EMPTY_NORMAL;
            draw_board(false, true);
            play_sound(sounds[MOVE2]);
            SDL_Delay(400);
            if (last_position != move.y)
              current_board[last_position] = EMPTY_NORMAL;
          }
        }
      }
    }
    // Pawn Promotion Check
    if ((origin_piece == WHITE_PAWN && move.y/column_count == 0) || (origin_piece == BLACK_PAWN && move.y/column_count == row_count-1)) {
      PieceColor color_mult = origin_piece/COLOR_DIVIDER;
      current_board[move.y] = origin_piece;
      draw_board(false, true);
      SDL_Delay(300);
      play_sound(sounds[PUP]);
      current_board[move.y] = WHITE_QUEEN*color_mult;
    }
    else if (current_board[move.y] != origin_piece) {
      current_board[move.y] = origin_piece;
      play_sound(sounds[MOVE2]);
    }
    char *player = malloc(75);
    if (current_player == WHITE_PLAYER) {
      strcpy(player, "\t\tWhites Move: ");
      debug_log(true, __LINE__, __FILE__, strcat(player, "(%d, %d)\n\t\tWhite Pieces Left: %d\tBlack Pieces Left: %d\n\n"), move.x, move.y, WHITE_PIECES_COUNT, BLACK_PIECES_COUNT);
    }
    else {
      strcpy(player, "\t\tBlacks Move: ");
      debug_log(true, __LINE__, __FILE__, strcat(player, "(%d, %d)\n\t\tWhite Pieces Left: %d\tBlack Pieces Left: %d\n\n"), move.x, move.y, WHITE_PIECES_COUNT, BLACK_PIECES_COUNT);
    }
    free(player);
    strcat(game_output_text, msg_control[move.x]);
    strcat(game_output_text, " ");
    strcat(game_output_text, msg_control[move.y]);
    return switch_turn();
  }
  return check_game_state();
}

void undo_last_move() {
  SDL_LockMutex(join_mutex);
  if (local_ready == true) {
    SDL_UnlockMutex(join_mutex);
    return;
  }
  SDL_UnlockMutex(join_mutex);
  if (current_states >= 0) {
    for (int i = 0; i < get_board_size(); i++)
      current_board[i] = (previous_states[current_states])[i];
    current_states--;
    switch_turn();
    game_over = false;
    play_sound(sounds[BIP2]);
    char log[50] = "\t\tUndoing last move. ";
    debug_log(true, __LINE__, __FILE__, strcat(log, current_player == WHITE_PLAYER ? "Whites turn to play.\n\n" : "Blacks turn to play.\n\n"));
  }
}
