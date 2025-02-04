#ifndef __DOUBLY_LINKED_LIST_H__
#define __DOUBLY_LINKED_LIST_H__

struct Node {
  struct Node *next;
  struct Node *prev;
  int x, y;
};

struct Node *new_node(int x, int y);

struct Node *add_to_head(struct Node *head, int x, int y);

struct Node *add_to_tail(struct Node *tail, int x, int y);

#endif
