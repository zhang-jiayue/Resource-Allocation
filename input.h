/**
 *  input.h
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Representation of an input line.
 *  
 */


#ifndef INPUT_H
#define INPUT_H

// representation of line in the input file
typedef struct input {
    /*each thread utilizes a resource as follows:
    * 0: initialization
    * 1: request
    * 2: release
    * 3: terminate
    * 4: compute
    */
    int  type;  
    int  id;  //thread id
    int  Rn;  //resource type id
    int  units;
    int  compute_counter;
} Input;

#endif
