#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t *game = (game_state_t *)malloc(sizeof(game_state_t));
  
  game->num_rows = 18;
  unsigned int columns = 20;
  
  game->board = (char **)malloc(sizeof(char *) * game->num_rows);
  
  for (int i=0; i<game->num_rows; i++) {
    game->board[i] = (char *)malloc((columns + 1) * sizeof(char));
  }
  
  game->num_snakes = 1;
  game->snakes = (snake_t *)malloc(sizeof(snake_t));
  game->snakes->live = true;
  game->snakes->tail_row = 2;
  game->snakes->tail_col = 2;
  game->snakes->head_row = 2;
  game->snakes->head_col= 4;

  char *s[] = {
  "####################",
  "#                  #",
  "# d>D    *         #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "#                  #",
  "####################"
  };

  for (int i=0; i<game->num_rows; i++) {
    strcpy(game->board[i], s[i]);
  }

  return game;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  free(state->snakes);
  for (int i=0; i<state->num_rows; i++)
    free(state->board[i]);
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for(int i=0; i<state->num_rows; i++)
    fprintf(fp,"%s\n", state->board[i]);
  
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  char *s = "wasd";
  if (strchr(s, c))
    return true;
  else
    return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  char *s = "WASDx";
  if (strchr(s, c))
    return true;
  else
    return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  char *s = "wasd^<v>WASDx";
  if (strchr(s, c))
    return true;
  else
    return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c) {
    case '^': return 'w'; break;
    case '<': return 'a'; break;
    case 'v': return 's'; break;
    case '>': return 'd'; break;
    default: return '?';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch(c) {
    case 'W': return '^'; break;
    case 'A': return '<'; break;
    case 'S': return 'v'; break;
    case 'D': return '>'; break;
    default: return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  switch(c) {
    case 'v':
    case 's':
    case 'S': return cur_row + 1; break;
    case '^':
    case 'w':
    case 'W': return cur_row - 1; break;
    default: return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  switch(c) {
    case '>':
    case 'd':
    case 'D': return cur_col + 1; break;
    case '<':
    case 'a':
    case 'A': return cur_col - 1; break;
    default: return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state->snakes[snum];
  unsigned int col = snake->head_col;
  unsigned int row = snake->head_row;
  char **board = state->board;
  char head = board[row][col];
  
  switch (head)
  {
    case 'W': return board[row-1][col]; break;
    case 'A': return board[row][col-1]; break;
    case 'S': return board[row+1][col]; break;
    case 'D': return board[row][col+1]; break;
    default: return '?';
  }
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state->snakes[snum];
  unsigned int col = snake->head_col;
  unsigned int row = snake->head_row;
  char **board = state->board;
  char head = board[row][col];
  
  switch (head)
  {
    case 'W': snake->head_row -= 1, board[row][col] = '^', board[row-1][col] = 'W'; break;
    case 'A': snake->head_col -= 1, board[row][col] = '<', board[row][col-1] = 'A'; break;
    case 'S': snake->head_row += 1, board[row][col] = 'v', board[row+1][col] = 'S'; break;
    case 'D': snake->head_col += 1, board[row][col] = '>', board[row][col+1] = 'D'; break;
    default: ;
  }
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t *snake = &state->snakes[snum];
  unsigned int col = snake->tail_col;
  unsigned int row = snake->tail_row;
  char **board = state->board;
  char tail = board[row][col];
  
  switch (tail)
  {
    case 'w': snake->tail_row -= 1, board[row][col] = ' ', board[row-1][col] = body_to_tail(board[row-1][col]); break;
    case 'a': snake->tail_col -= 1, board[row][col] = ' ', board[row][col-1] = body_to_tail(board[row][col-1]); break;
    case 's': snake->tail_row += 1, board[row][col] = ' ', board[row+1][col] = body_to_tail(board[row+1][col]); break;
    case 'd': snake->tail_col += 1, board[row][col] = ' ', board[row][col+1] = body_to_tail(board[row][col+1]); break;
    default: ;
  }
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  char *bodycell = "wasdWASD^<v>#";
  for (unsigned int i=0; i<state->num_snakes; i++) {
    if (strchr(bodycell, next_square(state, i))) {
      unsigned int col = state->snakes[i].head_col;
      unsigned int row = state->snakes[i].head_row;
      state->snakes[i].live = false;
      state->board[row][col] = 'x';
    } else if(next_square(state, i) == '*') {
      add_food(state);
      update_head(state, i);
    } else {
      update_head(state, i);
      update_tail(state, i);
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
    if (fp == 0) {
      fprintf(stdout, "fp is not a file, %p", fp);
      fflush(stdout);
      return NULL;
    }
    char c = (char )fgetc(fp);
    game_state_t *state = (game_state_t *)malloc(sizeof(game_state_t));
    state->num_rows = 0;
    state->num_snakes = 0;
    state->snakes = NULL;
    char **board = state->board;

    board = (char **)calloc(state->num_rows+1, sizeof(char *));

    // char *buf = (char *)malloc(sizeof(char) * 10);
    unsigned int num_cols = 0;
    for (; c != EOF; ) {
        // printf("%c", c);
        // fflush(stdout);
        if (c != '\n') {
            board[state->num_rows] = (char *)realloc(board[state->num_rows], sizeof(char) * (num_cols+1));
            board[state->num_rows][num_cols] = c;
            num_cols++;
        } else {
            board[state->num_rows] = (char *)realloc(board[state->num_rows], sizeof(char) * (num_cols+1));
            board[state->num_rows][num_cols] = '\0';
            state->num_rows++;
            board = (char **)realloc(board, sizeof(char *) * (state->num_rows + 1));
            memset(board+state->num_rows, 0, sizeof(char *));
            num_cols = 0;
        }
        c = (char )fgetc(fp);
    }
    state->board = board;
    
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  return NULL;
}
