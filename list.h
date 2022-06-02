/**
 *  list.h
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Description of the program: an SLL.
 *  
 */
 
#include "task.h"

struct node {
    Task *task;
    struct node *next;
    struct node *last;
    int size;
};

// insert and delete operations.
void insert(struct node **head, Task *task);
struct node* delete(struct node **sentinel, Task **t);