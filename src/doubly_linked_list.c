#include "doubly_linked_list.h"

#include <stdlib.h>

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
