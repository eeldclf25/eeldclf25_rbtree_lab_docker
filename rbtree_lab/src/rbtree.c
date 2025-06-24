#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>





void left_rotate(rbtree *t, node_t *target_node) {
  node_t *child_node = target_node->right;

  target_node->right = child_node->left;
  if (child_node->left != t->nil)
    child_node->left->parent = target_node;

  child_node->parent = target_node->parent;
  if (target_node == target_node->parent->right)
    target_node->parent->right = child_node;
  else if (target_node == target_node->parent->left)
    target_node->parent->left = child_node;
  else
    t->root = child_node;

  child_node->left = target_node;
  target_node->parent = child_node;
}

void right_rotate(rbtree *t, node_t *target_node) {
  node_t *child_node = target_node->left;

  target_node->left = child_node->right;
  if (child_node->right != t->nil)
    child_node->right->parent = target_node;

  child_node->parent = target_node->parent;
  if (target_node == target_node->parent->left)
    target_node->parent->left = child_node;
  else if (target_node == target_node->parent->right)
    target_node->parent->right = child_node;
  else
    t->root = child_node;

  child_node->right = target_node;
  target_node->parent = child_node;
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

void delete_node(rbtree *t, node_t *node)
{
  if (node == t->nil)
    return;
  delete_node(t, node->left);
  delete_node(t, node->right);
  free(node);
}

void delete_rbtree(rbtree *t)
{
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  //0. 새로 넣을 노드 생성-------------------------------------------------------------------------------------------------------------------------------
  node_t *new_node = malloc(sizeof(node_t));
  new_node->color = RBTREE_RED;
  new_node->key = key;
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->parent = t->nil;

  //1. BST 노드 삽입-------------------------------------------------------------------------------------------------------------------------------
  node_t **insert_node = &t->root;  // 노드를 담은 포인터의 포인터를 생성
  node_t **insert_parent_node = &t->nil;
  while (*insert_node != t->nil) {
    insert_parent_node = insert_node;
    insert_node = (new_node->key >= (*insert_node)->key) ? (&(*insert_node)->right) : (&(*insert_parent_node)->left);
  }
  *insert_node = new_node;  // 이중포인터를 이용하면 우리가 넣어야 할 위치의 "공간"을 while문 하나로 찾을 수 있음
  new_node->parent = *insert_parent_node;

  //2. RB트리의 균형 조정-------------------------------------------------------------------------------------------------------------------------------
  node_t *check_node = new_node;  // 새로 넣은 노드(맨 아래)에서부터 트리 균형 맞추기 시작
  node_t *check_parent_node = check_node->parent;
  node_t *check_grand_node = check_parent_node->parent;
  while (check_parent_node->color == RBTREE_RED) {  // 삽입에서의 균형 기준은, 부모가 Red여서 Red to Red가 되는 경우에만 계속 균형을 맞춰주면 됨
    if (check_grand_node->left->color == RBTREE_RED && check_grand_node->right->color == RBTREE_RED) {
      check_grand_node->color = RBTREE_RED; // 부모 및 삼촌이 Red이고 할아버지가 Black일 경우, 이를 부모 및 삼촌을 Black, 할아버지를 Red로 바꿔도 Black Height를 유지할 수 있게 됨
      check_grand_node->left->color = RBTREE_BLACK;
      check_grand_node->right->color = RBTREE_BLACK;
      check_node = check_grand_node;  // 유일하게 회전없이 할아버지 노드 그대로 Red로 만들어서 Red to Red 가능성이 있기 때문에 반복해야 함
      check_parent_node = check_grand_node->parent;
      check_grand_node = check_grand_node->parent->parent;
    }
    else {  // 삼촌이 아닌 부모만 Red일 경우, 회전을 이용하여 Black Height를 유지하면서 Red to Red가 안되기 때문에 한번만 수행하면 됨
      if (check_grand_node->left == check_parent_node) {
        if (check_parent_node->right == check_node)
          left_rotate(t, check_parent_node);
        right_rotate(t, check_grand_node);
      }
      else if (check_grand_node->right == check_parent_node) {
        if (check_parent_node->left == check_node)
          right_rotate(t, check_parent_node);
        left_rotate(t, check_grand_node);
      }
      check_grand_node->color = RBTREE_RED;
      check_grand_node->parent->color = RBTREE_BLACK;
      break;  // 회전을 이용한 재조정은 Red to Red 가 불가능 하기 때문에 종료
    }
  }

  //3. 루트는 무조건 Black-------------------------------------------------------------------------------------------------------------------------------
  t->root->color = RBTREE_BLACK;
  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *check_node = t->root;
  while (check_node != t->nil) {
    if (key > check_node->key)
      check_node = check_node->right;
    else if (key < check_node->key)
      check_node = check_node->left;
    else
      return check_node;
  }
  return NULL;
}

node_t *rbnode_min_max(const rbtree *t, node_t *p, bool is_min)
{
  node_t *return_node = p;
  while ((is_min ? return_node->left : return_node->right) != t->nil)
    return_node = (is_min ? return_node->left : return_node->right);
  return return_node;
}

node_t *rbtree_min(const rbtree *t)
{
  node_t *return_node = rbnode_min_max(t, t->root, true);
  return return_node;
}

node_t *rbtree_max(const rbtree *t)
{
  node_t *return_node = rbnode_min_max(t, t->root, false);
  return return_node;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  //0. 삭제할 노드 및 extra black이 될 노드를 저장할 변수 생성-------------------------------------------------------------------------------------------------------------------------------
  node_t *delete_node = p;
  node_t *extra_node = NULL;

  //1. 삭제-------------------------------------------------------------------------------------------------------------------------------
  if (delete_node->right != t->nil && delete_node->left != t->nil) {
    node_t *temp = rbnode_min_max(t, delete_node->left, false);
    delete_node->key = temp->key;
    delete_node = temp;
  }
  if (delete_node->right != t->nil) {
    extra_node = delete_node->right;
    delete_node->right->parent = delete_node->parent;
    if (delete_node->parent->right == delete_node) {
      delete_node->parent->right = delete_node->right;
    }
    else if (delete_node->parent->left == delete_node) {
      delete_node->parent->left = delete_node->right;
    }
    else {
      t->root = delete_node->right;
    }
  }
  else if (delete_node->left != t->nil) {
    extra_node = delete_node->left;
    delete_node->left->parent = delete_node->parent;
    if (delete_node->parent->right == delete_node) {
      delete_node->parent->right = delete_node->left;
    }
    else if (delete_node->parent->left == delete_node) {
      delete_node->parent->left = delete_node->left;
    }
    else {
      t->root = delete_node->left;
    }
  }
  else if (delete_node->left == t->nil && delete_node->right == t->nil) {
    if (delete_node->parent->left == delete_node) {
      delete_node->parent->left = t->nil;
      t->nil->parent = delete_node->parent;
      extra_node = t->nil;
    }
    else if (delete_node->parent->right == delete_node) {
      delete_node->parent->right = t->nil;
      t->nil->parent = delete_node->parent;
      extra_node = t->nil;
    }
    else {
      t->root = t->nil;
      extra_node = t->nil;
    }
  }


  if (delete_node->color != RBTREE_RED) {
    while (extra_node != t->root && extra_node->color == RBTREE_BLACK) {
      node_t *extra_parent_node = extra_node->parent;
      node_t *extra_brother_node = (extra_parent_node->left == extra_node) ? extra_parent_node->right : extra_parent_node->left;

      if (extra_brother_node->color == RBTREE_BLACK
      && extra_brother_node->left->color == RBTREE_BLACK
      && extra_brother_node->right->color == RBTREE_BLACK) {
        extra_brother_node->color = RBTREE_RED;
        extra_node = extra_node->parent;
      }
      else {
        if (extra_parent_node->left == extra_node) {
          if (extra_brother_node->color == RBTREE_RED
          && extra_brother_node->left->color == RBTREE_BLACK
          && extra_brother_node->right->color == RBTREE_BLACK) {
            extra_parent_node->color = RBTREE_RED;
            extra_brother_node->color = RBTREE_BLACK;
            left_rotate(t, extra_parent_node);
          }
          else if (extra_brother_node->color == RBTREE_BLACK
          && extra_brother_node->left->color == RBTREE_RED
          && extra_brother_node->right->color == RBTREE_BLACK) {
            extra_brother_node->color = RBTREE_RED;
            extra_brother_node->left->color = RBTREE_BLACK;
            right_rotate(t, extra_brother_node);
          }
          else if (extra_brother_node->color == RBTREE_BLACK
          && extra_brother_node->right->color == RBTREE_RED) {
            extra_brother_node->color = extra_parent_node->color;
            extra_parent_node->color = RBTREE_BLACK;
            extra_brother_node->right->color = RBTREE_BLACK;
            left_rotate(t, extra_parent_node);
            break;
          }
        }
        else {
          if (extra_brother_node->color == RBTREE_RED
          && extra_brother_node->left->color == RBTREE_BLACK
          && extra_brother_node->right->color == RBTREE_BLACK) {
            extra_parent_node->color = RBTREE_RED;
            extra_brother_node->color = RBTREE_BLACK;
            right_rotate(t, extra_parent_node);
          }
          else if (extra_brother_node->color == RBTREE_BLACK
          && extra_brother_node->left->color == RBTREE_BLACK
          && extra_brother_node->right->color == RBTREE_RED) {
            extra_brother_node->color = RBTREE_RED;
            extra_brother_node->right->color = RBTREE_BLACK;
            left_rotate(t, extra_brother_node);
          }
          else if (extra_brother_node->color == RBTREE_BLACK
          && extra_brother_node->left->color == RBTREE_RED) {
            extra_brother_node->color = extra_parent_node->color;
            extra_parent_node->color = RBTREE_BLACK;
            extra_brother_node->left->color = RBTREE_BLACK;
            right_rotate(t, extra_parent_node);
            break;
          }
        }
      }
    }
  }


  extra_node->color = RBTREE_BLACK;
  t->root->color = RBTREE_BLACK;
  t->nil->parent = t->nil;
  free(delete_node);
  return 0;
}

size_t rbtree_to_array_recursion(const rbtree *t, key_t *arr, node_t *node, size_t count, size_t max)
{
  if (node == t->nil || count >= max)
    return count;

  size_t left_count = rbtree_to_array_recursion(t, arr, node->left, count, max);
  if (left_count < max)
    arr[left_count++] = node->key;
  size_t right_count = rbtree_to_array_recursion(t, arr, node->right, left_count, max);
  
  return right_count;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  rbtree_to_array_recursion(t, arr, t->root, 0, n);
  return 0;
}