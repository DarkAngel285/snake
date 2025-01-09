#include "game.h"

struct Node {
  struct Node *next;
  struct Node *prev;
  int x, y;
};

struct Node *new_node(int x, int y) {
  struct Node *node = (struct Node *)malloc(sizeof(struct Node));
  node->next = NULL;
  node->prev = NULL;
  node->x = x;
  node->y = y;
  return node;
}

struct Node *add_to_head(struct Node *head, int x, int y) {
  head->prev = new_node(x, y);
  head->prev->next = head;
  return head->prev;
}

struct Node *add_to_tail(struct Node *tail, int x, int y) {
  tail->next = new_node(x, y);
  tail->next->prev = tail;
  return tail->next;
}

int snake_check(struct Node *head, int direction, struct winsize w) {
  if ((head->y == 1 && direction == 0) || (head->x == 1 && direction == 3) ||
      (head->y == w.ws_row && direction == 1) ||
      (head->x == w.ws_col && direction == 2)) {
    return 1;
  }
  int x = head->x, y = head->y;
  while (head) {
    if ((y - 1 == head->y && x == head->x && direction == 0) ||
        (y + 1 == head->y && x == head->x && direction == 1) ||
        (x - 1 == head->x && y == head->y && direction == 3) ||
        (x + 1 == head->x && y == head->y && direction == 2))
      return 1;
    head = head->next;
  }
  return 0;
}

void snake_move(struct Node *tail, int direction, struct winsize w) {
  while (tail) {
    if (tail->prev != NULL) {
      tail->x = tail->prev->x;
      tail->y = tail->prev->y;
    } else {
      if (1 < tail->y && direction == 0)
        tail->y -= 1;
      else if (tail->y < w.ws_row && direction == 1)
        tail->y += 1;
      else if (tail->x < w.ws_col && direction == 2)
        tail->x += 1;
      else if (1 < tail->x && direction == 3)
        tail->x -= 1;
    }
    tail = tail->prev;
  }
}

void snake_print(struct Node *head, int x, int y) {
  if (head->x != x || head->y != y) {
    clear();
    while (head) {
      gotoxy(head->x, head->y);
      printf("%s@%s", (!head->prev ? GREEN : YELLOW), STD);
      head = head->next;
    }
    fflush(stdout);
  }
}

void apple_check(struct Node *head, struct Node **tail, unsigned int *seed,
                 struct winsize w, int *x, int *y, unsigned int *score) {
  if (head->x == *x && head->y == *y) {
    *tail = add_to_tail(*tail, (*tail)->x, (*tail)->y);
    *x = rand_r(seed) % w.ws_col + 1;
    *y = rand_r(seed) % w.ws_row + 1;
    *score += 1;
  }
  if (*x == 0)
    (*x)++;
  if (*y == 0)
    (*y)++;
}

void apple_print(int x, int y) {
  gotoxy(x, y);
  printf("%s#%s", RED, STD);
  fflush(stdout);
}

void game(unsigned int *score) {
  unsigned int seed = time(0);

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  char c[MAXBYTES];

  int direction = 2;
  int prev_x = 1, prev_y = 1;
  struct Node *head = new_node(1, 1), *tail = new_node(1, 1);
  head->next = tail;
  tail->prev = head;

  int sleep_time = STDSLEEPTIME;
  int apple_x = rand_r(&seed) % w.ws_col, apple_y = rand_r(&seed) % w.ws_row;

  while (1) {
    int bytes_read = read(STDIN_FILENO, &c, MAXBYTES);
    if (bytes_read != 0) {
      if (((c[0] == '\x1b' && c[1] == '[' && c[2] == 'A') || c[0] == 'w') &&
          direction != 1) {
        direction = 0;
      } else if (((c[0] == '\x1b' && c[1] == '[' && c[2] == 'B') ||
                  c[0] == 's') &&
                 direction != 0) {
        direction = 1;
      } else if (((c[0] == '\x1b' && c[1] == '[' && c[2] == 'C') ||
                  c[0] == 'd') &&
                 direction != 3) {
        direction = 2;
      } else if (((c[0] == '\x1b' && c[1] == '[' && c[2] == 'D') ||
                  c[0] == 'a') &&
                 direction != 2) {
        direction = 3;
      } else if (0 <= c[0] - '0' && c[0] - '0' <= 9) {
        sleep_time = STDSLEEPTIME - (1e4 * (c[0] - '0'));
      } else if (c[0] == 'q') {
        break;
      }
    }

    if (snake_check(head, direction, w))
      break;
    snake_move(tail, direction, w);
    snake_print(head, prev_x, prev_y);
    prev_x = head->x;
    prev_y = head->y;

    apple_check(head, &tail, &seed, w, &apple_x, &apple_y, score);
    apple_print(apple_x, apple_y);

    usleep(sleep_time);
  }

  struct Node *next = NULL;
  while (head) {
    next = head->next;
    free(head);
    head = next;
  }
}

void deinit(struct termios *oldt, unsigned int score) {
  clear();
  printf("Your score: %u\n", score);
  fflush(stdout);
  curhide(0);
  tcsetattr(STDIN_FILENO, TCSANOW, oldt);
}

void init(struct termios *oldt, struct termios *newt) {
  curhide(1);
  clear();
  fflush(stdout);
  tcgetattr(STDIN_FILENO, oldt);
  *newt = *oldt;
  newt->c_lflag &= ~(ICANON | ECHO);
  newt->c_cc[VMIN] = 0;
  newt->c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, newt);
}

int main() {
  unsigned int score = 0;
  struct termios oldt, newt;

  init(&oldt, &newt);
  game(&score);
  deinit(&oldt, score);
  return 0;
}
