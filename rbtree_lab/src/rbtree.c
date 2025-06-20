#include "rbtree.h"

#include <stdlib.h>
#include <stdbool.h>





void rotate_LR_RL(rbtree *t, node_t *grand_node, bool is_lr) {
  node_t *parent_node = is_lr ? grand_node->left : grand_node->right;
  node_t *target_node = is_lr ? parent_node->right : parent_node->left;
  node_t *temp_target_child = is_lr ? target_node->left : target_node->right;

  if (is_lr) {
    target_node->left = parent_node;
    parent_node->right = temp_target_child;
    grand_node->left = target_node;
  }
  else {
    target_node->right = parent_node;
    parent_node->left = temp_target_child;
    grand_node->right = target_node;
  }

  if (temp_target_child != t->nil) temp_target_child->parent = parent_node;
  parent_node->parent = target_node;
  target_node->parent = grand_node;
}

void rotate_LL_RR(rbtree *t, node_t *grand_node, bool is_ll) {
  node_t *grand_grand_node = grand_node->parent;
  node_t *parent_node = is_ll ? grand_node->left : grand_node->right;
  node_t *temp_parent_child = is_ll ? parent_node->right : parent_node->left; 

  if (is_ll) {
    parent_node->right = grand_node;
    grand_node->left = temp_parent_child;
  }
  else {
    parent_node->left = grand_node;
    grand_node->right = temp_parent_child;
  }

  if (temp_parent_child != t->nil) temp_parent_child->parent = grand_node;
  parent_node->parent = grand_node->parent;
  grand_node->parent = parent_node;

  if (grand_grand_node == t->nil) t->root = parent_node;
  else if (grand_node == grand_grand_node->left) grand_grand_node->left = parent_node;
  else if (grand_node == grand_grand_node->right) grand_grand_node->right = parent_node;
}





rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  node_t *nilNode = malloc(sizeof(node_t));
  nilNode->color = RBTREE_BLACK;
  nilNode->key = 0;
  nilNode->left = nilNode;
  nilNode->right = nilNode;
  nilNode->parent = nilNode;

  p->nil = nilNode;
  p->root = nilNode;
  return p;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  node_t *new_node = malloc(sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->parent = t->nil;

  node_t **insert_node = &t->root;
  node_t *insert_parent_node = t->nil;
  while (*insert_node != t->nil)
  {
    insert_parent_node = *insert_node;
    insert_node = (new_node->key >= insert_parent_node->key) ? &insert_parent_node->right : &insert_parent_node->left;
  }
  *insert_node = new_node;
  new_node->parent = insert_parent_node;

  node_t *check_node = new_node;
  node_t *check_parent_node = check_node->parent;
  node_t *check_grand_node = check_parent_node->parent;
  while (check_parent_node->color == RBTREE_RED) {
    if (check_grand_node->left->color == RBTREE_RED && check_grand_node->right->color == RBTREE_RED)
    {
      check_grand_node->left->color = RBTREE_BLACK;
      check_grand_node->right->color = RBTREE_BLACK;
      check_grand_node->color = RBTREE_RED;
      check_node = check_grand_node;
      check_parent_node = check_grand_node->parent;
      check_grand_node = check_grand_node->parent->parent;
      continue;
    }

    if (check_grand_node->left == check_parent_node) {
      if (check_grand_node->left->right == check_node)
        rotate_LR_RL(t, check_grand_node, true);
      rotate_LL_RR(t, check_grand_node, true);
    }
    else if (check_grand_node->right == check_parent_node) {
      if (check_grand_node->right->left == check_node)
        rotate_LR_RL(t, check_grand_node, false);
      rotate_LL_RR(t, check_grand_node, false);
    }
    check_grand_node->color = RBTREE_RED;
    check_grand_node->parent->color = RBTREE_BLACK;
    break;
  }

  t->root->color = RBTREE_BLACK;
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *check_node = t->root;
  while (check_node != t->nil) {
    if (key == check_node->key) {
      return check_node;
    }
    else if (key >= check_node->key) {
      check_node = check_node->right;
    }
    else {
      check_node = check_node->left;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  return 0;
}
