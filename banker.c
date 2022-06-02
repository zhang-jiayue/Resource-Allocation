/**
 * Full Name: Jiayue Zhang
 * Course ID: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 * Description: a simple optimistic resource manager and a resource manager that avoids deadlock using banker's algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for strcmp

#include "banker.h"

int num_of_tasks, num_of_resources;
// [FreeResources] Current free resources for each type. free_resources[0] is for R1, etc.
int free_resources[MAX];
// An array of all the tasks
Task tasks[MAX];
// A wait queue of Tasks
struct node *waiting = NULL;
int total_compute = 0;
int main(int argc, char *argv[])
{
	FILE *fp;
	fp = fopen(argv[1], "r");
	// read the # of threads and resource types in system
	fscanf(fp, "%d %d ", &num_of_tasks, &num_of_resources);

	// An array of struct Input, stores the values of each rows in the input file
	Input *inputs = malloc(num_of_tasks * sizeof(Input));

	/* read inputs */
	int i;
	for (i = 0; i < num_of_resources; i++)
	{
		int tmp;
		fscanf(fp, "%d", &tmp);
		free_resources[i] = tmp;
	}
	char myBuffer[10];
	int total_requests;
	total_requests = 0;
	while (fscanf(fp, "%10s", myBuffer) != EOF)
	{
		fscanf(fp, "%d %d %d",
			   &inputs[total_requests].id,
			   &inputs[total_requests].Rn,
			   &inputs[total_requests].units);
		inputs[total_requests].compute_counter = inputs[total_requests].Rn;
		if (!strcmp(myBuffer, "initiate"))
		{
			inputs[total_requests].type = 0;
			tasks[inputs[total_requests].id].num_of_initiate++;
		}
		else if (!strcmp(myBuffer, "request"))
		{
			inputs[total_requests].type = 1;
		}
		else if (!strcmp(myBuffer, "release"))
		{
			inputs[total_requests].type = 2;
		}
		else if (!strcmp(myBuffer, "terminate"))
		{
			inputs[total_requests].type = 3;
		}
		else if (!strcmp(myBuffer, "compute"))
		{
			inputs[total_requests].type = 4;
			total_compute += inputs[total_requests].Rn;
		}
		total_requests++;
	}

	/* Add each input lines to all the tasks */
	int j;
	for (j = 0; j < total_requests; j++)
	{
		int tmp = tasks[inputs[j].id].num_of_requests;
		tasks[inputs[j].id].requests[tmp] = inputs[j];
		tasks[inputs[j].id].num_of_requests++;
	}

	/**
	 * @brief Optimistic resource manager
	 * Satisfy a request if possible, if not, put this Task on the wait queue.
	 * When a release occurs, dequeue in FIFO manner.
	 */
	int finished = 0; // 0: not finished 1: finished
	int cycle;
	// During each cycle, the threads run in parallel,
	// first iterate through each task,
	// and find the next pending request in its requests array.
	for (cycle = 0; cycle < total_requests + total_compute && !finished; cycle++)
	{
		int task_counter;
		int pending;
		int not_on_the_queue[MAX];
		// This task terminated or released resources in the last cycle, release all its resources.
		for (task_counter = 1; task_counter <= num_of_tasks; task_counter++)
		{
			Task *cur = &tasks[task_counter];
			if ((pending = cur->next_pending_request) == -1)
			{
				int i;
				for (i = 0; i < num_of_resources; i++)
				{
					free_resources[i] += cur->alloc[i];
					cur->alloc[i] = 0;
				}
				cur->next_pending_request = -3; //-3:terminated task resources released
			}
			else if (cur->released)
			{
				Input last_request = cur->requests[cur->next_pending_request - 1];
				free_resources[last_request.Rn - 1] += last_request.units;
				cur->alloc[last_request.Rn - 1] -= last_request.units;
			}
		}

		/* not dequeued in this cycle */
		for (task_counter = 1; task_counter <= num_of_tasks; task_counter++)
		{
			not_on_the_queue[task_counter] = 0;
		}
		int counter = 1;
		while (counter <= num_of_tasks)
		{
			if (!(tasks[counter].enqueued) && (tasks[counter].next_pending_request != -1) && (tasks[counter].next_pending_request != -2) && (tasks[counter].next_pending_request != -3))
			{
				not_on_the_queue[counter] = 1;
			}
			counter++;
		}

		/* Check the wait queue */
		struct node *ptr = waiting;
		while (ptr != NULL)
		{ // iterate through the wait queue to see if any can be granted
			Task *t = ptr->task;
			if (!grant(t))
			{ // take it off the wait queue
				delete (&waiting, &t);
				t->next_pending_request++;
			}
			else
			{
				t->fifo_waiting++;
			}
			ptr = ptr->next;
		}

		/* Check other tasks that are not on the queue */
		for (task_counter = 1; task_counter < MAX; task_counter++)
		{
			if (not_on_the_queue[task_counter] == 1)
			{
				Task *cur = &tasks[task_counter];
				cur->released = 0;
				pending = cur->next_pending_request;
				Input next_request = cur->requests[pending];
				int request_type = next_request.type;
				if (request_type == 0)
				{
					// If the next pending request for this task is initializaiton, execute immediately
					// each initilize request takes one CPU cycle
					// ignore initial claim
					cur->next_pending_request++;
				}
				else if (request_type == 1)
				{
					// If request, grant the resource if possible.
					// If not granted, put the thread on the wait queue and run detect_deadlock function,
					// abort tasks starting from the smallest id if it returns 1.
					if (grant(cur))
					{ // cur not granted, put it on the wait queue
						insert(&waiting, cur);
						cur->fifo_waiting++;
					}
					else
					{ // granted
						cur->next_pending_request++;
					}
				}
				else if (request_type == 2)
				{
					cur->released = 1;
					// If release, add to free_resources in the next cycle.
					cur->next_pending_request++;
				}
				else if (request_type == 4)
				{ // compute
					if (cur->requests[pending].compute_counter > 0)
					{
						cur->requests[pending].compute_counter--;
					}
					if (cur->requests[pending].compute_counter == 0)
					{
						cur->next_pending_request++;
					}
				}
				else
				{ // utilizaiton_type==3
					// If terminate, free allocated resources in the next cycle, set next_pending_request to -1.
					cur->fifo_total = cycle;
					cur->next_pending_request = -1;
				}
			}

		} // end for task_counter
		finished = all_finished();
		/* deadlock detection*/
		if (!finished && detect_deadlock())
		{
			abort();
		}
	} // end cycle
	  /* reset tasks */
	for (i = 1; i <= num_of_tasks; i++)
	{
		tasks[i].enqueued = 0;
		tasks[i].released = 0;
		int j;
		for (j = 0; j < num_of_resources; j++)
		{
			tasks[i].alloc[j] = 0;
		}
	}
	/**
	 * @brief Banker's algorithm
	 * Evaluate each request and grant if some ordering of threads is still deadlock free afterward.
	 * prevent system from reaching an unsafe state.
	 */
	finished = 0;
	for (cycle = 0; cycle < total_requests + total_compute && !finished; cycle++)
	{
		if (cycle == 0)
		{
			abort_banker();
		}
		int task_counter;
		int pending;
		int not_on_the_queue[MAX];
		// This task terminated or released resources in the last cycle, release all its resources.
		for (task_counter = 1; task_counter <= num_of_tasks; task_counter++)
		{
			Task *cur = &tasks[task_counter];
			if ((pending = cur->next_pending_request_banker) == -1)
			{
				int i;
				for (i = 0; i < num_of_resources; i++)
				{
					free_resources[i] += cur->alloc[i];
					cur->alloc[i] = 0;
				}
				cur->next_pending_request_banker = -3; //-3:terminated task resources released
			}
			else if (cur->released)
			{
				Input last_request = cur->requests[cur->next_pending_request_banker - 1];
				free_resources[last_request.Rn - 1] += last_request.units;
				cur->alloc[last_request.Rn - 1] -= last_request.units;
			}
		}

		/* not dequeued in this cycle */
		for (task_counter = 1; task_counter <= num_of_tasks; task_counter++)
		{
			not_on_the_queue[task_counter] = 0;
		}
		int counter = 1;
		while (counter <= num_of_tasks)
		{
			if (!(tasks[counter].enqueued) && (tasks[counter].next_pending_request_banker != -1) && (tasks[counter].next_pending_request_banker != -2) && (tasks[counter].next_pending_request_banker != -3))
			{
				not_on_the_queue[counter] = 1;
			}
			counter++;
		}

		/* Check the wait queue */
		struct node *ptr = waiting;
		while (ptr != NULL)
		{ // iterate through the wait queue to see if any is safe after request
			Task *t = ptr->task;
			if (is_safe(t))
			{ // take it off the wait queue
				banker_grant(t);
				delete (&waiting, &t);
				t->next_pending_request_banker++;
			}
			else
			{
				t->banker_waiting++;
			}
			ptr = ptr->next;
		}

		/* Check other tasks that are not on the queue */
		for (task_counter = 1; task_counter < MAX; task_counter++)
		{
			if (not_on_the_queue[task_counter] == 1)
			{
				Task *cur = &tasks[task_counter];
				cur->released = 0;
				pending = cur->next_pending_request_banker;
				Input next_request = cur->requests[pending];
				int request_type = next_request.type;
				if (request_type == 0)
				{
					// If the next pending request for this task is initializaiton, execute immediately
					// each initilize request takes one CPU cycle
					// ignore initial claim
					cur->next_pending_request_banker++;
				}
				else if (request_type == 1)
				{
					// If request, grant the resource if possible.
					// If not granted, put the thread on the wait queue and run detect_deadlock function,
					// abort tasks starting from the smallest id if it returns 1.
					if (!is_safe(cur))
					{ // cur not granted, put it on the wait queue
						insert(&waiting, cur);
						cur->banker_waiting++;
					}
					else
					{ // granted
						banker_grant(cur);
						cur->next_pending_request_banker++;
					}
				}
				else if (request_type == 2)
				{
					cur->released = 1;
					// If release, add to free_resources in the next cycle.
					cur->next_pending_request_banker++;
				}
				else if (request_type == 4)
				{ // compute
					if (cur->requests[pending].Rn > 0)
					{
						cur->requests[pending].Rn--;
					}
					if (cur->requests[pending].Rn == 0)
					{
						cur->next_pending_request_banker++;
					}
				}
				else
				{ // utilizaiton_type==3
					// If terminate, free allocated resources in the next cycle, set next_pending_request to -1.
					cur->banker_total = cycle;
					cur->next_pending_request_banker = -1;
				}
			}

		} // end for task_counter
		finished = all_finished_banker();
		/* deadlock detection*/
	} // end cycle

	/**
	 * @brief Output:
	 *
	 */
	char *sptr = "FIFO";
	char *sptr2 = "BANKER'S";
	int fifo_all_tasks_total = 0;
	int fifo_total_waiting = 0;
	int banker_all_tasks_total = 0;
	int banker_total_waiting = 0;
	printf("%23s%33s\n", sptr, sptr2);
	for (i = 1; i <= num_of_tasks; i++)
	{
		char *fifo = (char *)malloc(50 * sizeof(char));
		char *banker = (char *)malloc(50 * sizeof(char));
		if (tasks[i].next_pending_request == -2)
		{
			strcpy(fifo, " aborted");
		}
		if (tasks[i].next_pending_request_banker == -2)
		{
			strcpy(banker, " aborted");
		}
		if (tasks[i].next_pending_request != -2)
		{
			fifo_all_tasks_total += tasks[i].fifo_total;
			fifo_total_waiting += tasks[i].fifo_waiting;
			sprintf(fifo, "%2d%6d%6d%%",
					tasks[i].fifo_total, tasks[i].fifo_waiting, (int)(100 * ((float)tasks[i].fifo_waiting / tasks[i].fifo_total + 0.005)));
		}
		if (tasks[i].next_pending_request_banker != -2)
		{

			banker_all_tasks_total += tasks[i].banker_total;
			banker_total_waiting += tasks[i].banker_waiting;
			sprintf(banker, "%2d%6d%6d%%",
					tasks[i].banker_total, tasks[i].banker_waiting, (int)(100 * ((float)tasks[i].banker_waiting / tasks[i].banker_total + 0.005)));
		}
		printf("%11s%2d%3s%-15s%12s%2d%3s%-25s\n", "Task", i, " ",fifo, "Task", i, " ", banker);
	}
	char *fifo = (char *)malloc(50 * sizeof(char));
	sprintf(fifo, "%2d%6d%6d%%",
			fifo_all_tasks_total, fifo_total_waiting, (int)(100 *((float)fifo_total_waiting / fifo_all_tasks_total + 0.005)));
	char *banker = (char *)malloc(50 * sizeof(char));
	sprintf(banker, "%2d%6d%6d%%",
			banker_all_tasks_total, banker_total_waiting, (int)(100 * ((float)banker_total_waiting / banker_all_tasks_total + 0.005)));
	printf("%12s%4s%-11s%13s%4s%-11s\n", "total", " ",
		   fifo, "total", " ", banker);

	fclose(fp);

	return 0;
} // end main

/**
 * @brief A simple deadlock detection alg:
 * if all tasks have outstanding requests that the manager cannot satisfy,
 * @return 1 to indicate a deadlock.
 * @return 0 if it is not going to deadlock.
 */
int detect_deadlock()
{
	int i;
	int resu = 1;
	for (i = 1; i <= num_of_tasks; i++)
	{
		int pending = tasks[i].next_pending_request;
		if (pending != -2 && pending != -3)
		{ // if all tasks not terminated or aborted are enqueued
			resu = resu && tasks[i].enqueued;
		}
	}
	return resu;
} // end detect_deadlock

/**
 * @return 1 to indicate all finished.
 */
int all_finished()
{
	int i;
	int resu = 1;
	for (i = 1; i <= num_of_tasks; i++)
	{
		int pending = tasks[i].next_pending_request;
		resu = resu && (pending == -1 || pending == -2 || pending == -3);
	}
	return resu;
} // end detect_deadlock

int all_finished_banker()
{
	int i;
	int resu = 1;
	for (i = 1; i <= num_of_tasks; i++)
	{
		int pending = tasks[i].next_pending_request_banker;
		resu = resu && (pending == -1 || pending == -2 || pending == -3);
	}
	return resu;
} // end detect_deadlock
/**
 * @brief abort the lowest numbered deadlocked task
 * once aborted, the task does not get to run again
 */
void abort()
{
	while (!check())
	{
		int i;
		for (i = 1; i <= num_of_tasks; i++)
		{
			int next = tasks[i].next_pending_request;
			if (next != -1 && next != -2 && next != -3 && tasks[i].enqueued)
			{
				break;
			}
		}
		Task *t = &tasks[i];
		t->next_pending_request = -2;
		// release its resources now to check if need to abort another task in this cycle
		for (i = 0; i < num_of_resources; i++)
		{
			free_resources[i] += t->alloc[i];
			t->alloc[i] = 0;
		}
		if (t->enqueued == 1)
		{
			// take the task off the wait queue
			delete (&waiting, &t);
		}
	}
}

void abort_banker()
{
	int i;
	for (i = 1; i <= num_of_tasks; i++)
	{
		Task *t = &tasks[i];
		int j;
		for (j = 0; j < t->num_of_initiate; j++)
		{
			int present = free_resources[t->requests[j].Rn - 1];
			int claim = t->requests[j].units;
			if (claim > present)
			{
				t->next_pending_request_banker = -2;
				printf("  Banker aborts task %d before run begins:\n       claim for resource %d (%d) exceeds number of units present (%d)\n", i, t->requests[j].Rn, claim, present);
			}
		}
	}
}

/**
 * @return 0 if granted, 1 if not
 */
int grant(Task *task)
{
	int pending = task->next_pending_request;
	Input next_request = task->requests[pending];
	int r = next_request.Rn;
	if (next_request.units <= free_resources[r - 1])
	{
		free_resources[r - 1] -= next_request.units;
		tasks[next_request.id].alloc[r - 1] += next_request.units;
		return 0;
	}
	return 1;
}

/**
 * @brief check the wait queue to see if any of them is possible to run without allocating resources.
 * @return int 1: false 0: there exists an  task whose outstanding request can be satisfied
 */
int check()
{
	int i;
	for (i = 1; i <= num_of_tasks; i++)
	{
		Task t = tasks[i];
		int pending = t.next_pending_request;
		Input next_request = t.requests[pending];
		if (pending != -2 && pending != -1 && pending != -3 && next_request.units <= free_resources[next_request.Rn - 1])
		{
			return 1;
		}
	}
	return 0;
}

/**
 * @brief Check if a request is safe
 * @return 0 unsafe
 * @return 1 safe
 */
int is_safe(Task *task)
{ // If for each resource type, the task's initial claim [Initial - Alloc] <= [Avail],
  // then it is safe
	int i;
	int resu = 1;
	for (i = 0; i < task->num_of_initiate; i++)
	{
		int resource_type = task->requests[i].Rn - 1;
		resu = resu &&  (task->requests[i].units - task->alloc[resource_type] <= free_resources[resource_type]);
	}
	return resu;
} // end is_safe

int banker_grant(Task *task)
{
	int pending = task->next_pending_request_banker;
	Input next_request = task->requests[pending];
	int r = next_request.Rn;
	free_resources[r - 1] -= next_request.units;
	tasks[next_request.id].alloc[r - 1] += next_request.units;
	return 0;
}
