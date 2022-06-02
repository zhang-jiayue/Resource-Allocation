/**
 *  list.c
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Description of the program: An SLL with only two operations insert to the front of the list and delete a node with task t in the list.
 */
 
#include <stdlib.h>
#include <stdio.h>

#include "list.h"

//add to the end of the list
void insert(struct node **head, Task *newTask) {
    struct node *newNode = malloc(sizeof(struct node));
    struct node *temp;
    struct node *prev;
    temp = *head;
    prev = *head;
    newNode->task = newTask;
    newNode->next = NULL;
    newNode->task->enqueued = 1;
    // Speical case: empty list
    if ((*head) == NULL) {
        (*head) = newNode;
    } else {
        while (temp!= NULL) {
	        prev = temp;
            temp = temp->next;
        }
        prev->next = newNode;
    }
}

// delete a node with attribute Task* t, and returns the node
struct node* delete(struct node **head, Task **t) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - empty list
    if ((*head) == NULL) {
        return (*head);
    }
    // special case - first element in the list
    if (temp->next == NULL && temp->task== (*t)) {
        (*head) = NULL;
        temp->task->enqueued = 0;
        return (*head);
    }
    if (temp->next!= NULL && temp->task== (*t)) {
        (*head) = (*head)->next;
        temp->task->enqueued = 0;
        return temp; 
    }
    else {
        // any element in the list
        prev = *head;
        while (temp!=NULL && temp->task!= (*t)) {
            prev = temp;
            temp = temp->next;
        }
        if (temp!=NULL && temp->task == (*t)) {
            temp->task->enqueued = 0;
            prev->next = temp->next;
        }
        return temp;    //if not found returns NULL
    }
}


