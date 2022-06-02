/**
 *  task.h
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Representation of a task requiring resources.
 *  
 */


#ifndef TASK_H
#define TASK_H
 /* max size of the waiting queue, 
 * the max number of tasks and the max number of requests that each task can make. */
#define MAX 20  

#include "input.h"

// representation of a thread
typedef struct task {
    Input requests[MAX];   // current requests from thread, assume the maximum # of requests for each task is MAX.
    int num_of_requests;
    int next_pending_request;   // -1: this task already terminated -2: task aborted
    int next_pending_request_banker;
    int alloc[MAX];// resources held by thread currently
    int enqueued; //0: not on the queue 1:enqueued
    int released;   // 1: released resources in the last cycle
    int fifo_waiting;
    int fifo_total;
    int banker_waiting;
    int banker_total;
    int num_of_initiate;
} Task;

#endif


