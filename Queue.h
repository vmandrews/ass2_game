#ifndef H_QUEUE_H
#define H_QUEUE_H

#include "Places.h"
#include "GameView.h"

typedef struct queue *qlist;
typedef struct trapVam *TrapVam;

typedef struct node{
    LocationID playerloc;
    struct node *next;
    struct node *prev;
}node;

typedef struct queue{
    node *head;
    node *tail;
    int size;
}queue;

typedef struct trapVam{
    qlist trap;
    qlist vam;
}trapVam;

//returns an empty queue
qlist init();

//returns the number of elements in the q
//can also be used to check if q is empty
int isNotEmpty(qlist q);

//this is a stack pop, pops the element last went into the q
LocationID spop(qlist q);

//this is a normal q pop
LocationID pop(qlist q);

//peeks the element at postion n counting from back to front(starting from 1)
//e.g. we have 0 1 2 3 4 5 6 7 in the q
//peek(q,1) == 7; peek(q,2) == 6; peek(q,3) == 5
LocationID peek(qlist q, int n);

//push an element at the front of the q
void push(qlist q, LocationID it);

//push a node in
void pushtrap(qlist q, node *newt);

//remove a particular element
void movetrap(qlist q, LocationID place);

//count the number of traps at place
//trapType 0 = trap;1 = vampire
int trapNum(TrapVam tv, LocationID place, int trapType);

//updates trap status at place
//type indicates what kind of update
//0 = encounter (so a hunter steped onto the trap) so all traps at location place disappear
//1 = place(so dracular made a new trap) insert new trap
//2 = mature(so trap has matured) so the oldest trap dissapear
void updateTrap(TrapVam tv,LocationID place,int type);

//same as above but vampire
void updateVam(TrapVam tv, LocationID place, int type);

//free the trap and vampire
void freeTravm(TrapVam ts);

#endif
