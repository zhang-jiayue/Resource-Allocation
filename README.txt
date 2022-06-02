DEPS : banker.h list.h task.h input.h
banker.h: header file for banker.c
list.h: defines a node in a DLL.
list.c: an SLL with only two operations insert to the front of the list and delete a node with task t in the list.
task.h: Representation of a task requiring resources. 
        Defines MAX, which is the max number of tasks, the max number of requests each task can make, and also the max number of tasks that can join the wait queue.
        currently set to 20, can be changed to any other integer.
input.h:    Representation of an input line, which is a request that a task makes.
            The attribute type converts the first string in each line to an integer as follows:
            * 0: initialization
            * 1: request
            * 2: release
            * 3: terminate
            * 4: compute
banker.c: a simple optimistic resource manager and a resource manager that avoids deadlock using banker's algorithm.