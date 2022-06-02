/**
 *  banker.h
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Description of the program: header file for banker.c.
 *  
 */

 /* max size of the waiting queue, 
 * the number of tasks and the max number of requests that each task can make. */
#include "list.c"

int detect_deadlock();
void abort();
void abort_banker();
int grant(Task *task);
int check();
int all_finished();
int all_finished_banker();
int is_safe(Task *t);
int banker_grant(Task *task);



